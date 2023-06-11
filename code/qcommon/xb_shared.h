

// Peter: For colored themes
#define C_DEFAULT	0
#define C_BLACK		1
#define C_RED		2
#define C_GREEN		3
#define C_YELLOW	4
#define C_BLUE		5
#define C_CYAN		6
#define C_MAGENTA	7
#define C_WHITE		8

#define SAY_CHAT        0
#define SAY_TEAM        1
#define SAY_TELL        2
#define SAY_SAY         3
#define SAY_PRIVATE     4

/* Macros */
#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= (bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))

#define URANGE(a, b, c)          ((b) < (a) ? (a) : ((b) > (c) ? (c) : (b)))

#define KEY( literal, field, value ) \
if( !strcmp( word, (literal) ) )    \
{                                    \
   (field) = (value);                \
   fMatch = qtrue;                    \
   break;                            \
}

#define CREATE(result, type, number)					\
do											\
{											\
    if (!((result) = (type *) calloc ((number), sizeof(type))))	\
    {											\
      perror("calloc failure");                                         \
      fprintf(stderr, "Calloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
	abort();									\
    }											\
} while(0)

#define DISPOSE(point)                         \
do                                             \
{                                              \
   if( (point) )                               \
   {                                           \
         free( (point) );                      \
      (point) = NULL;                          \
   }                                           \
   else                                          \
      (point) = NULL;                            \
} while(0)

#define REMOVE_FROM_LIST(item, head, next)	\
   if ((item) == (head))		\
      head = (item)->next;		\
   else {				\
      temp = head;			\
      while (temp && (temp->next != (item))) \
	 temp = temp->next;		\
      if (temp)				\
         temp->next = (item)->next;	\
   }					\
 

/* double-linked list handling macros */
#define LINK(link, first, last, next, prev)                     	\
do                                                              	\
{                                                               	\
   if ( !(first) )								\
   {                                           				\
      (first) = (link);				                       	\
      (last) = (link);							    	\
   }											\
   else                                                      	\
      (last)->next = (link);			                       	\
   (link)->next = NULL;			                         	\
   if ((first) == (link))							\
      (link)->prev = NULL;							\
   else										\
      (link)->prev = (last);			                       	\
   (last) = (link);				                       	\
} while(0)

#define INSERT(link, insert, first, next, prev)                 \
do                                                              \
{                                                               \
   (link)->prev = (insert)->prev;			                \
   if ( !(insert)->prev )                                       \
      (first) = (link);                                         \
   else                                                         \
      (insert)->prev->next = (link);                            \
   (insert)->prev = (link);                                     \
   (link)->next = (insert);                                     \
} while(0)

#define UNLINK(link, first, last, next, prev)                   	\
do                                                              	\
{                                                               	\
	if ( !(link)->prev )							\
	{			                                    	\
         (first) = (link)->next;			                 	\
	   if ((first))							 	\
	      (first)->prev = NULL;						\
	} 										\
	else										\
	{                                                 		\
         (link)->prev->next = (link)->next;                 	\
	}										\
	if ( !(link)->next ) 							\
	{				                                    \
         (last) = (link)->prev;                 			\
      if((last))                                \
	      (last)->next = NULL;						\
	} 										\
	else										\
	{                                                    		\
         (link)->next->prev = (link)->prev;                 	\
	}										\
} while(0)

#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

/* xb_math.c */
void init_genrand(unsigned long s);
unsigned long genrand_int32(void);
int rand_number(int from, int to);
int dice(int num, int size);

/* xb_shared.c */
qboolean is_number( char *arg );
void half_chop(char *string, char *arg1, char *arg2);
