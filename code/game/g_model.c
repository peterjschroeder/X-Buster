//
// Eltran's Model Code Porting
//

#include "g_local.h"

model_t   	*loadmodel;
trGlobals_t 	tra;


typedef struct
{
    int      mark;
    int      permanent;
    int      temp;
    int      tempHighwater;
} hunkUsed_t;

int         	fs_loadStack;         // total files in memory
byte   		*s_hunkData = NULL;
hunkUsed_t   	*hunk_permanent, *hunk_temp;
hunkUsed_t   	hunk_low, hunk_high;
int      	s_hunkTotal;


#define        LL(x) x=LittleLong(x)

/*
=================
Hunk_Alloc

Allocate permanent (until the hunk is cleared) memory
=================
*/
static void Hunk_SwapBanks( void )
{
    hunkUsed_t   *swap;

    // can't swap banks if there is any temp already allocated
    if ( hunk_temp->temp != hunk_temp->permanent )
    {
        return;
    }

    // if we have a larger highwater mark on this side, start making
    // our permanent allocations here and use the other side for temp
    if ( hunk_temp->tempHighwater - hunk_temp->permanent >
            hunk_permanent->tempHighwater - hunk_permanent->permanent )
    {
        swap = hunk_temp;
        hunk_temp = hunk_permanent;
        hunk_permanent = swap;
    }
}

int FS_LoadStack()
{
    return fs_loadStack;
}

void Hunk_Clear( void )
{

    hunk_low.mark = 0;
    hunk_low.permanent = 0;
    hunk_low.temp = 0;
    hunk_low.tempHighwater = 0;

    hunk_high.mark = 0;
    hunk_high.permanent = 0;
    hunk_high.temp = 0;
    hunk_high.tempHighwater = 0;

    hunk_permanent = &hunk_low;
    hunk_temp = &hunk_high;

    Com_Printf( "Hunk_Clear: reset the hunk ok\n" );
}

/*
=================
Com_InitZoneMemory
=================
*/
void Com_InitHunkMemory( void )
{
    int   cv;
    int nMinAlloc;
    char *pMsg = NULL;

    // make sure the file system has allocated and "not" freed any temp blocks
    // this allows the config and product id files ( journal files too ) to be loaded
    // by the file system without redunant routines in the file system utilizing different
    // memory systems
    if (FS_LoadStack() != 0)
    {
        Com_Error( ERR_FATAL, "Hunk initialization failed. File system load stack not zero");
    }

    // allocate the stack based hunk allocator
    cv = 56;

    // if we are not dedicated min allocation is 56, otherwise min is 1
    if (g_dedicated.integer)
    {
        nMinAlloc = 1;
        pMsg = "Minimum com_hunkMegs for a dedicated server is %i, allocating %i megs.\n";
    }
    else
    {
        nMinAlloc = 56;
        pMsg = "Minimum com_hunkMegs is %i, allocating %i megs.\n";
    }

    if ( cv < nMinAlloc )
    {
        s_hunkTotal = 1024 * 1024 * nMinAlloc;
        Com_Printf(pMsg, nMinAlloc, s_hunkTotal / (1024 * 1024));
    }
    else
    {
        s_hunkTotal = cv * 1024 * 1024;
    }


    // bk001205 - was malloc
    s_hunkData = calloc( s_hunkTotal + 31, 1 );
    if ( !s_hunkData )
    {
        Com_Error( ERR_FATAL, "Hunk data failed to allocate %i megs", s_hunkTotal / (1024*1024) );
    }
    // cacheline align
    s_hunkData = (byte *) ( ( (int)s_hunkData + 31 ) & ~31 );
    Hunk_Clear();
}

void *Hunk_Alloc( int size, ha_pref preference )
{
    void   *buf;

    if ( s_hunkData == NULL)
    {
        Com_Error( ERR_FATAL, "Hunk_Alloc: Hunk memory system not initialized" );
    }

    // can't do preference if there is any temp allocated
    if (preference == h_dontcare || hunk_temp->temp != hunk_temp->permanent)
    {
        Hunk_SwapBanks();
    }
    else
    {
        if (preference == h_low && hunk_permanent != &hunk_low)
        {
            Hunk_SwapBanks();
        }
        else if (preference == h_high && hunk_permanent != &hunk_high)
        {
            Hunk_SwapBanks();
        }
    }

    // round to cacheline
    size = (size+31)&~31;

    if ( hunk_low.temp + hunk_high.temp + size > s_hunkTotal )
    {
        Com_Error( ERR_DROP, "Hunk_Alloc failed on %i", size );
    }

    if ( hunk_permanent == &hunk_low )
    {
        buf = (void *)(s_hunkData + hunk_permanent->permanent);
        hunk_permanent->permanent += size;
    }
    else
    {
        hunk_permanent->permanent += size;
        buf = (void *)(s_hunkData + s_hunkTotal - hunk_permanent->permanent );
    }

    hunk_permanent->temp = hunk_permanent->permanent;

    //memset( buf, 0, size );
    return buf;
}

/*
** G_AllocModel
*/
model_t *G_AllocModel( void )
{
    model_t      *mod;

    if ( tra.numModels == MAX_MOD_KNOWN )
    {
        return NULL;
    }

    mod = Hunk_Alloc( sizeof( *tra.models[tra.numModels] ), h_low );
    mod->index = tra.numModels;
    tra.models[tra.numModels] = mod;
    tra.numModels++;

    return mod;
}

/*
=================
G_LoadMD3
=================
*/
static qboolean G_LoadMD3 (model_t *mod, int lod, char *buffer, const char *mod_name )
{
    int               i, j;
    md3Header_t       *pinmodel;
    md3Frame_t        *frame;
    md3Surface_t      *surf;
    md3Triangle_t     *tri;
    md3St_t           *st;
    md3XyzNormal_t    *xyz;
    md3Tag_t          *tag;
    int               version;
    int               size;

    pinmodel = (md3Header_t *)buffer;
    version = LittleLong (pinmodel->version);

    if (version != MD3_VERSION)
    {
        G_Printf("G_LoadMD3: %s has wrong version (%i should be %i)\n",
                 mod_name, version, MD3_VERSION);
        return qfalse;
    }

    mod->type = MOD_MESH;
    size = LittleLong(pinmodel->ofsEnd);
    mod->dataSize += size;
    mod->md3[lod] = Hunk_Alloc( size, h_low );

    memcpy(mod->md3[lod], buffer, LittleLong(pinmodel->ofsEnd) );

    LL(mod->md3[lod]->ident);
    LL(mod->md3[lod]->version);
    LL(mod->md3[lod]->numFrames);
    LL(mod->md3[lod]->numTags);
    LL(mod->md3[lod]->numSurfaces);
    LL(mod->md3[lod]->ofsFrames);
    LL(mod->md3[lod]->ofsTags);
    LL(mod->md3[lod]->ofsSurfaces);
    LL(mod->md3[lod]->ofsEnd);

    if ( mod->md3[lod]->numFrames < 1 )
    {
        G_Printf( "G_LoadMD3: %s has no frames\n", mod_name );
        return qfalse;
    }

    // swap all the frames
    frame = (md3Frame_t *) ( (byte *)mod->md3[lod] + mod->md3[lod]->ofsFrames );
    for ( i = 0 ; i < mod->md3[lod]->numFrames ; i++, frame++)
    {
        frame->radius = LittleFloat( frame->radius );
        for ( j = 0 ; j < 3 ; j++ )
        {
            frame->bounds[0][j] = LittleFloat( frame->bounds[0][j] );
            frame->bounds[1][j] = LittleFloat( frame->bounds[1][j] );
            frame->localOrigin[j] = LittleFloat( frame->localOrigin[j] );
        }
    }

    // swap all the tags
    tag = (md3Tag_t *) ( (byte *)mod->md3[lod] + mod->md3[lod]->ofsTags );
    for ( i = 0 ; i < mod->md3[lod]->numTags * mod->md3[lod]->numFrames ; i++, tag++)
    {
        for ( j = 0 ; j < 3 ; j++ )
        {
            tag->origin[j] = LittleFloat( tag->origin[j] );
            tag->axis[0][j] = LittleFloat( tag->axis[0][j] );
            tag->axis[1][j] = LittleFloat( tag->axis[1][j] );
            tag->axis[2][j] = LittleFloat( tag->axis[2][j] );
        }
    }

    // swap all the surfaces
    surf = (md3Surface_t *) ( (byte *)mod->md3[lod] + mod->md3[lod]->ofsSurfaces );
    for ( i = 0 ; i < mod->md3[lod]->numSurfaces ; i++)
    {

        LL(surf->ident);
        LL(surf->flags);
        LL(surf->numFrames);
        LL(surf->numShaders);
        LL(surf->numTriangles);
        LL(surf->ofsTriangles);
        LL(surf->numVerts);
        LL(surf->ofsShaders);
        LL(surf->ofsSt);
        LL(surf->ofsXyzNormals);
        LL(surf->ofsEnd);

        if ( surf->numVerts > SHADER_MAX_VERTEXES )
        {
            G_Error ("G_LoadMD3: %s has more than %i verts on a surface (%i)",
                     mod_name, SHADER_MAX_VERTEXES, surf->numVerts );
        }
        if ( surf->numTriangles*3 > SHADER_MAX_INDEXES )
        {
            G_Error ("G_LoadMD3: %s has more than %i triangles on a surface (%i)",
                     mod_name, SHADER_MAX_INDEXES / 3, surf->numTriangles );
        }

        // change to surface identifier
        surf->ident = SF_MD3;

        // lowercase the surface name so skin compares are faster
        Q_strlwr( surf->name );

        // strip off a trailing _1 or _2
        // this is a crutch for q3data being a mess
        j = strlen( surf->name );
        if ( j > 2 && surf->name[j-2] == '_' )
        {
            surf->name[j-2] = 0;
        }

        // swap all the triangles
        tri = (md3Triangle_t *) ( (byte *)surf + surf->ofsTriangles );
        for ( j = 0 ; j < surf->numTriangles ; j++, tri++ )
        {
            LL(tri->indexes[0]);
            LL(tri->indexes[1]);
            LL(tri->indexes[2]);
        }

        // swap all the ST
        st = (md3St_t *) ( (byte *)surf + surf->ofsSt );
        for ( j = 0 ; j < surf->numVerts ; j++, st++ )
        {
            st->st[0] = LittleFloat( st->st[0] );
            st->st[1] = LittleFloat( st->st[1] );
        }

        // swap all the XyzNormals
        xyz = (md3XyzNormal_t *) ( (byte *)surf + surf->ofsXyzNormals );
        for ( j = 0 ; j < surf->numVerts * surf->numFrames ; j++, xyz++ )
        {
            xyz->xyz[0] = LittleShort( xyz->xyz[0] );
            xyz->xyz[1] = LittleShort( xyz->xyz[1] );
            xyz->xyz[2] = LittleShort( xyz->xyz[2] );

            xyz->normal = LittleShort( xyz->normal );
        }


        // find the next surface
        surf = (md3Surface_t *)( (byte *)surf + surf->ofsEnd );
    }

    return qtrue;
}

/*
====================
RE_RegisterModel

Loads in a model for the given name

Zero will be returned if the model fails to load.
An entry will be retained for failed models as an
optimization to prevent disk rescanning if they are
asked for again.
====================
*/
qhandle_t G_RegisterModel( const char *name )
{
    model_t      *mod;
    char   *buf;
    int         lod;
    qboolean   loaded;
    qhandle_t   hModel;
    int         numLoaded;
    fileHandle_t f;
    int         len;

    if ( !name || !name[0] )
    {
        G_Printf( "G_RegisterModel: NULL name\n" );
        return 0;
    }

    if ( strlen( name ) >= MAX_QPATH )
    {
        Com_Printf( "Model name exceeds MAX_QPATH\n" );
        return 0;
    }

    //
    // search the currently loaded models
    //
    for ( hModel = 1 ; hModel < tra.numModels; hModel++ )
    {
        mod = tra.models[hModel];
        if ( !strcmp( mod->name, name ) )
        {
            if( mod->type == MOD_BAD )
            {
                return 0;
            }
            return hModel;
        }
    }

    // allocate a new model_t

    if ( ( mod = G_AllocModel() ) == NULL )
    {
        G_Printf( "G_RegisterModel: G_AllocModel() failed for '%s'\n", name);
        return 0;
    }

    // only set the name after the model has been successfully loaded
    Q_strncpyz( mod->name, name, sizeof( mod->name ) );

    mod->numLods = 0;

    //
    // load the files
    //
    numLoaded = 0;

    for ( lod = MD3_MAX_LODS - 1 ; lod >= 0 ; lod-- )
    {
        char filename[1024];

        strcpy( filename, name );

        //if ( lod != 0 ) {
        //   char namebuf[80];

        //   if ( strrchr( filename, '.' ) ) {
        //      *strrchr( filename, '.' ) = 0;
        //   }
        //   //sprintf( namebuf, "_%d.md3", lod );
        //   //strcat( filename, namebuf );
        //}

        len = trap_FS_FOpenFile(filename,&f,FS_READ);
        if ( !f )
        {
            G_Printf("FAILED!!! %s doesn't exist.\n",filename);
            return 0;
        }
        if ( !len )
        {
            //empty file
            G_Printf("FAILED!!! Empty file.\n");
            trap_FS_FCloseFile( f );
            return 0;
        }
        if ( (buf = G_Alloc(len+1)) == 0 )
        {
            //alloc memory for buffer
            G_Printf("FAILED!!! Unable to alloc buffer.\n");
            return 0;
        }
        trap_FS_Read( buf, len, f );
        if ( !buf )
        {
            continue;
        }

        loadmodel = mod;

        loaded = G_LoadMD3( mod, lod, buf, name );

        trap_FS_FCloseFile(f);

        if ( !loaded )
        {
            if ( lod == 0 )
            {
                mod->type = MOD_BAD;
                return 0;
            }
        }
        else
        {
            mod->numLods++;
            numLoaded++;
        }
    }

    if ( numLoaded )
    {
        // duplicate into higher lod spots that weren't
        // loaded, in case the user changes r_lodbias on the fly
        for ( lod-- ; lod >= 0 ; lod-- )
        {
            mod->numLods++;
            mod->md3[lod] = mod->md3[lod+1];
        }
        return mod->index;
    }
    else
    {
        G_Printf ("G_RegisterModel: couldn't load %s\n", name);
    }
    return 0;
}

/*
** G_GetModelByHandle
*/
model_t   *G_GetModelByHandle( qhandle_t index )
{
    model_t      *mod;

    // out of range gets the defualt model
    if ( index < 1 || index >= tra.numModels )
    {
        return tra.models[0];
    }

    mod = tra.models[index];

    return mod;
}

/*
====================
G_ModelBounds
====================
*/
void G_ModelBounds( qhandle_t handle, vec3_t mins, vec3_t maxs )
{
    model_t      *model;
    md3Header_t   *header;
    md3Frame_t   *frame;

    model = G_GetModelByHandle( handle );

    if ( !model->md3[0] )
    {
        VectorClear( mins );
        VectorClear( maxs );
        return;
    }

    header = model->md3[0];

    frame = (md3Frame_t *)( (byte *)header + header->ofsFrames );

    VectorCopy( frame->bounds[0], mins );
    VectorCopy( frame->bounds[1], maxs );
}

