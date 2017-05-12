
/*
 * THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND, 
 * AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH 
 * (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL 
 * WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
 * ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
 * RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
 * JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR 
 * DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the 
 * generality of the foregoing, neither the author, nor JAIR, nor JAIR's
 * publishers and distributors, warrant that the Source Code will be 
 * error-free, will operate without interruption, or will meet the needs 
 * of the user.
 */


/*********************************************************************
 * File: ff.h
 * Description: Types and structures for the Metric-FastForward planner.
 *
 *        --------- PDDL2.1 level 2 :: VERSION  v 1.0 --------------
 *
 * Author: Joerg Hoffmann 2001
 * Contact: hoffmann@informatik.uni-freiburg.de
 *
 *********************************************************************/ 








#ifndef __FF_H
#define __FF_H






#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <sys/times.h>









/*
 *  ------------------------------------ DEFINES ----------------------------
 */











/***********************
 * MEANINGLESS HELPERS *
 ***********************/




/* strcmp returns 0 if two strings are equal, which is not nice */
#define SAME 0









/****************
 * PARSING ETC. *
 ****************/









/* various defines used in parsing
 */
#define HIDDEN_STR "#"
#define AXIOM_STR "AXIOM"
#define NAME_STR "name\0"
#define VARIABLE_STR "variable\0"
#define STANDARD_TYPE "OBJECT\0"
#define EITHER_STR "EITHER"









/***************************
 * SOME ARBITRARY SETTINGS *
 ***************************/







/* maximal string length
 */
#define MAX_LENGTH 256 


/* marks border between connected items 
 */
#define CONNECTOR "~"


/* size of goals_at array in 1P extraction
 */
#define RELAXED_STEPS_DEFAULT 25


/* size of hash table for repeated states checking
 * during EHC breadth first search
 */
#define EHC_HASH_SIZE 8192
#define EHC_HASH_BITS 8191


/* size of hash table for repeated states checking
 * in plan construction
 */
#define PLAN_HASH_SIZE 1024
#define PLAN_HASH_BITS 1023


/* size of hash table for repeated states checking
 * during BFS search
 */
#define BFS_HASH_SIZE 65536
#define BFS_HASH_BITS 65535


/* cut random values of facts off modulo this value,
 * to make state sums fit into a single integer
 */
#define BIG_INT 1500000


/* max number of different fluents in one list of LNF
 */
#define MAX_LNF_F 500


/* max number of comps in one cond / precond / goal
 */
#define MAX_LNF_COMPS 500


/* max number of lnf effects in one action effect
 */
#define MAX_LNF_EFFS 500







/************************
 * INSTANTIATION LIMITS *
 ************************/








#define MAX_CONSTANTS 1000
#define MAX_PREDICATES 5000
#define MAX_FUNCTIONS 5000
#define MAX_TYPES 100
#define MAX_ARITY 20
#define MAX_VARS 50


#define MAX_TYPE 400


#define MAX_OPERATORS 100000

#define MAX_CONSTRAINTS 2000

/* in DNF: AND with OR - sons - collect 'hitting set':
 * one son of each OR node. 
 *
 * this here is initial max number of such son s that can be collected
 * (grows dynamically, if required)
 */
#define MAX_HITTING_SET_DEFAULT 1000


#define MAX_TYPE_INTERSECTIONS 50


#define MAX_RELEVANT_FACTS 100000
#define MAX_RELEVANT_FLUENTS 100000






/******************************************
 * DOMAIN STRUCTURE AND SEARCHING LIMITS *
 ******************************************/






/*#define MAX_STATE 800*/
#define MAX_STATE 2000


#define MAX_PLAN_LENGTH 100000







/****************
 * CODE DEFINES *
 ****************/









/* not a real 'code' define; used in relax and search to encode
 * infinite level number / plan length
 */
#ifndef INFINITY
#define INFINITY -1
#endif







/* define boolean types if not allready defined
 */
#ifndef Bool
typedef unsigned char Bool;
#ifndef TRUE /* we assume that FALSE is also not defined */
#define TRUE 1
#define FALSE 0
#endif /* TRUE */
#endif /* Bool */


/* code a param number into a negative number and vice versa
 */
#define ENCODE_VAR( val ) (val * (-1)) - 1
#define DECODE_VAR( val ) (val + 1) * (-1)

#define GET_CONSTANT( val, pointer ) ( val >= 0 ) ? val : pointer->inst_table[DECODE_VAR( val )]


/* Check allocated memory
 */
#define CHECK_PTR(p) if (NULL == (p)) { \
  fprintf(stdout, "\n\aNO MEMORY in file %s:%d\n\n", __FILE__, __LINE__); \
  exit(1);}


/* add elapsed time from main local time vars to specified val
 */
#define TIME( val ) val += ( float ) ( ( end.tms_utime - start.tms_utime + \
					 end.tms_stime - start.tms_stime  ) / 100.0 )



/* for time windows literals
 */

#define actionMin 0.00
#define actionMax 10000.00

#define MAX_TIME 10000.00




/*
 *  ------------------------------ DATA STRUCTURES ----------------------------
 */











/*******************
 * GENERAL HELPERS *
 *******************/








/* all command switches
 */
struct _command_line {

  char path[MAX_LENGTH];
  char ops_file_name[MAX_LENGTH];
  char fct_file_name[MAX_LENGTH];
  int display_info;
  int debug;
  unsigned int beamsize;
  Bool optimize;
  Bool ehc;
  Bool adl;
  Bool grounded;
  Bool external;
  Bool breadth;
  Bool external_merge;
  Bool dijkstra;
  Bool branch_and_bound;
  Bool stop_at_first_solution;
  int g_weight;
  int h_weight;

};


typedef char *Token;












/***********
 * PARSING *
 ***********/










/* A list of strings
 */
typedef struct _TokenList {

  char *item;
  struct _TokenList *next;

} TokenList, *TokenList_pointer ;





/* list of string lists
 */
typedef struct _FactList {

  TokenList *item;
  struct _FactList *next;

} FactList;



/* structure to store  typed-list-of <name>/<variable>,
 * as they are declared in PDDL files
 */
typedef struct _TypedList {

  char *name;

  /* each item in this list is the name of a type which
   * our type is the union of (EITHER - types ...)
   *
   * usually, this will default to a single-item TokenList.
   */
  TokenList *type;
  /* after first sweep, this will contain the number in type table
   */
  int n;

  struct _TypedList *next;

} TypedList;



/* only needed to parse in the predicates and their arg
 * definitions
 */
typedef struct _TypedListList {

  char *predicate;

  TypedList *args;

  struct _TypedListList *next;

} TypedListList;



typedef enum _ExpConnective{FHEAD = 1000,
			    NUMBER,
			    MINUS,
			    AD,
                            SU, 
			    MU, 
			    DI} ExpConnective;



typedef struct _ParseExpNode {

  ExpConnective connective;

  /* NULL anywhere except when node is FHEAD or NUMBER
   * (in which case it is fn name ... resp. number (int or float) as string
   */
  TokenList *atom;

    int found;

  /* both NULL in FHEAD;
   * in MINUS, left is son and right is NULL
   * else (binary operators), left and right operand
   */
  struct _ParseExpNode *leftson, *rightson;

} ParseExpNode;



/* This type indicates whether a node in the pddl tree stands for
 * an atomic expression, a junctor or a quantor. 
 */
typedef enum _Connective{TRU = 2000,
			 FAL,
			 ATOM,
			 COMP,
                         TEMPORALOP,
			 NEF,
			 NOT, 
			 AND, 
			 OR, 
			 ALL, 
			 EX, 
			 WHEN,AT,START,END,OVER,ttw_AT,pref} Connective;



typedef enum _Comparator{IGUAL = 3000, /* technical if conds are array comp exp, resp float */
			 LE,
			 LEQ,
			 EQ,
			 GEQ,
			 GE} Comparator;




typedef enum _NumericEffectType{ASSIGN = 4000,
				SCALE_UP,
				SCALE_DOWN,
				INCREASE,
				DECREASE} NumericEffectType;

/*for constraints temporal operator
**/

typedef enum _TemporalConnective{AEND = 5000,
			 ALWAYS,
			 SOMETIME,
			 WITHIN,
                         AT_MOST_ONCE,
			 SOMETIME_AFTER,
			 SOMETIME_BEFORE, 
			 ALWAYS_WITHIN,
			 HOLD_DURING, 
			 HOLD_AFTER } TemporalConnective;


/*
 * This is a node in the tree to parse PDDL files
 */
typedef struct _PlNode {

  /* type of the node
   */
  Connective connective;
  TemporalConnective temporal_op; 
  int atom_t;


  Bool Pref;
  char *name;
  Bool at_end_found;

  /* only for parsing: the var args in quantifiers
   */
  TypedList *parse_vars;

  /* AND, OR, NOT, WHEN,
   * COMP, NEF         => NULL
   * ALL, EX            => the quantified variable with its type
   * ATOM               => the atom as predicate->param1->param2->...
   */
  TokenList *atom;
  /* all except COMP, NEF => NULL
   * COMP, NEF => left hand, right hand
   */
  Comparator comp;
  NumericEffectType neft;
  ParseExpNode *lh, *rh;

  /* (a) for AND, OR this is the list of sons(a AND b AND c...),
   * (b) for the rest this is the son, e.g. a subtree that is negated
   * (c) for WHEN, the first son is the condition and the next son
   * is the effect
   */
  struct _PlNode *sons;

  /* if you have a list of sons, they are connected by next
   */
  struct _PlNode *next;

} PlNode;



/*
 * This resembles an uninstantiated PDDL operator
 */
typedef struct _PlOperator {

  char *name;
  
  /* only important for PDDL where :VARS may be added to the param list
   * which must be hidden when writing the plan to an output file
   */
  int number_of_real_params; 

  /* the params, as they are declared in domain file
   */

  TypedList *parse_params;

  /* params is a list of variable/type pairs, such that:
   * factlist->item = [variable] -> [type]
   */
  FactList *params;
  PlNode *preconds;
  PlNode *effects;
  PlNode *ttw_preconds;
  ParseExpNode *duration;  

  struct _PlOperator *next;

} PlOperator;


/* Time windows literals
 */

/* Time windows literals with one interval
 */


/* 
typedef struct _Time_Ini_Literal {

  float maxtime;
  float mintime;

  TokenList *pre;
  
  struct _Time_Ini_Literal *next;

} Time_Ini_Literal;
*/







/* Time windows literals with multiple interval
 */

typedef struct _TimeWindows {

   float maxtime;
   float mintime;

   struct _TimeWindows *next;

} TimeWindows, *TimeWindows_pointer;

typedef struct _Time_Ini_Literal {

   int num;
   TimeWindows *tw;
   TokenList *pre;
   int numofint;
   struct _Time_Ini_Literal *next;

} Time_Ini_Literal;





/***************** 
 * INSTANTIATION *
 *****************/









/* helpers
 */

typedef int TypeArray[MAX_TYPE_INTERSECTIONS];

typedef int *int_pointer;




/* first step structures: parsing & preprocessing
 */

typedef struct _Fact {

  int predicate, args[MAX_ARITY];

/*----------temporale--Daten------------------*/

    int predicate_t;

    int prednorm_t;

/*-------end----temporale--Daten--------------*/

} Fact, *Fact_pointer;



typedef struct _Fluent {

  int function, args[MAX_ARITY];

    int function_t;

} Fluent;



typedef struct _FluentValue {

  Fluent fluent;
  float value;

} FluentValue;



typedef struct _Facts {

  Fact *fact;

  struct _Facts *next;

} Facts;



typedef struct _FluentValues {

  Fluent fluent;
  float value;

  struct _FluentValues *next;

} FluentValues;



typedef struct _ExpNode {

  ExpConnective connective;

  /* in FHEAD nodes, pre-processing
   */
  Fluent *fluent;
  /* in FHEAD nodes after pre-processes have finished.
   * (internal number of relevant fluent, or -1 if not
   * relevant)
   */
  int fl;
  /* helper for LNF: if that fl is multiplied, this is the
   * respective constant after pre-normalization.
   */
  float c;

  /* in NUMBER nodes
   */
  float value;


/*----------temporale--Daten------------------*/

    int lh_t;

/*-------end----temporale--Daten--------------*/

  /* in MINUS nodes
   */
  struct _ExpNode *son;

  /* in all others
   */
  struct _ExpNode *leftson, *rightson;

} ExpNode, *ExpNode_pointer;



typedef struct _WffNode {

  Connective connective;

  /* in ALL/EX s
   */
  int var, var_type;
  char *var_name;

  char *name;
  char *new_name;

  /* in AND/OR s
   */
  struct _WffNode *sons;
  /* sons are doubly connected linear list
   */
  struct _WffNode *next;
  struct _WffNode *prev;

  /* in ATOMs
   */
  Fact *fact;

    int fact_t;

  /* after translation: mark NOT-p s for efficiency
   */
  int NOT_p;

  /* in ALL/EX/NOT
   */
  struct _WffNode *son;

  /* in COMP
   */
  Comparator comp;
  ExpNode *lh, *rh;

  /*for constraints and preferences
  **/
  TemporalConnective temporal_op; 

  /* for expansion speedup
   */
  Bool visited;

  /* no WHEN s here... use Pl Connectives anyway for simplicity
   */

} WffNode, *WffNode_pointer;



typedef struct _Literal {

  Bool negated;

  Fact fact;

  int fact_t;

  struct _Literal *next;
  struct _Literal *prev;

} Literal;



typedef struct _NumericEffect {

  Fluent fluent;

  int fluent_t;

  NumericEffectType neft;

  ExpNode *rh;

  struct _NumericEffect *next;
  struct _NumericEffect *prev;

} NumericEffect;



typedef struct _Effect {

  int num_vars, var_types[MAX_VARS];
  char *var_names[MAX_VARS];

  WffNode *conditions;

  Literal *effects;
  NumericEffect *numeric_effects;

  struct _Effect *next;
  struct _Effect *prev;

} Effect;



typedef struct _Operator {

  char *name, *var_names[MAX_VARS];
  int number_of_real_params; 
 

  int num_vars, var_types[MAX_VARS];
  Bool removed[MAX_VARS];
 
  WffNode *preconds;
  WffNode *ttw_preconds;
  WffNode *pref_preconds;

  Effect *effects;

 ExpNode *operator_duration;

  Bool hard;

} Operator, *Operator_pointer;






/* second step: structures that keep already normalized
 * operators
 */




typedef struct _NormEffect {

   int num_vars, var_types[MAX_VARS];
  int inst_table[MAX_VARS];

  Fact *conditions;
  int num_conditions;

  Fact *adds;


  int num_adds;
  Fact *dels;


  int num_dels;

  /* numerical parts: not yet normalized any further; seems that
   * normalizing requires certain additional structures +
   * transformation, and that these will better be done when 
   * the representation is fully instantiated already.
   */
  Comparator *numeric_conditions_comp;
  ExpNode_pointer *numeric_conditions_lh, *numeric_conditions_rh;
  int num_numeric_conditions;

  NumericEffectType *numeric_effects_neft;
  Fluent *numeric_effects_fluent;
  ExpNode_pointer *numeric_effects_rh;
  int num_numeric_effects;

  struct _NormEffect *prev;
  struct _NormEffect *next;

} NormEffect;



typedef struct _NormOperator {
  
  Operator *operator;

  int num_vars, var_types[MAX_VARS];
  int inst_table[MAX_VARS];
  int removed_vars[MAX_VARS], num_removed_vars, type_removed_vars[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  Fact *ttw_preconds;
  int ttw_num_preconds;

  Fact *pref_preconds;
  int num_pref_preconds;

  /* numeric precondition still full scale represented, see above
   */
  Comparator *numeric_preconds_comp;
  ExpNode_pointer *numeric_preconds_lh, *numeric_preconds_rh;
  int num_numeric_preconds;

  NormEffect *effects;

ExpNode *normoperator_duration;

  Bool out;

} NormOperator, *NormOperator_pointer;
  


/* minimal info for a fully instantiated easy operator;
 * yields one action when expanded
 */
typedef struct _EasyTemplate {

  NormOperator *op;
  int inst_table[MAX_VARS];

  struct _EasyTemplate *prev;
  struct _EasyTemplate *next;

} EasyTemplate;






/* structures for hard ops
 */





/* intermediate step: structure for keeping hard ops
 * with normalized precondition, but arbitrary
 * effect conditions
 */
typedef struct _MixedOperator {
  
  Operator *operator;

  int inst_table[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  Fact *ttw_preconds;
  int ttw_num_preconds;

  ExpNode *operator_duration;

  /* numeric part, pre-normalized
   */
  Comparator *numeric_preconds_comp;
  ExpNode_pointer *numeric_preconds_lh, *numeric_preconds_rh;
  int num_numeric_preconds;

  Effect *effects;

  struct _MixedOperator *next;

} MixedOperator;



/* last hard step: everything is action - like, except that
 * facts are not yet integer coded
 */  



typedef struct _PseudoActionEffect {

  Fact *conditions;
  int num_conditions;

  Fact *adds;
  int num_adds;
  Fact *dels;
  int num_dels;


  /* and the numeric parts again...
   */
  Comparator *numeric_conditions_comp;
  ExpNode_pointer *numeric_conditions_lh, *numeric_conditions_rh;
  int num_numeric_conditions;

  NumericEffectType *numeric_effects_neft;
  Fluent *numeric_effects_fluent;
  ExpNode_pointer *numeric_effects_rh;
  int num_numeric_effects;

  struct _PseudoActionEffect *next;

} PseudoActionEffect;



typedef struct _PseudoAction {

  Operator *operator;

  int inst_table[MAX_VARS];

  Fact *preconds;
  int num_preconds;

  Fact *ttw_preconds;
  int ttw_num_preconds;

  /* numeric part, pre-normalized
   */
  Comparator *numeric_preconds_comp;
  ExpNode_pointer *numeric_preconds_lh, *numeric_preconds_rh;
  int num_numeric_preconds;

  PseudoActionEffect *effects;
  int num_effects;

  ExpNode *pseudo_duration;

} PseudoAction, *PseudoAction_pointer;




/* final domain representation structure
 */



typedef struct _LnfExpNode {

  int pF[MAX_LNF_F];
  float pC[MAX_LNF_F];
  int num_pF;

  int nF[MAX_LNF_F];
  float nC[MAX_LNF_F];
  int num_nF;

  float c;

} LnfExpNode, *LnfExpNode_pointer;



typedef struct _ActionEffect {

  int *conditions;
  int num_conditions;

  int *conditions_t;

  int *adds;
  int num_adds;

  int *adds_t;

  int *dels;
  int num_dels;

  int * dels_t;

  /* and the numeric parts again; fluents all as fl ints;
   *
   * normalization for cond as below for pre;
   * norm. for effects by restriction of types (?),
   * right hand side float (?)
   */
  Comparator *numeric_conditions_comp;
  ExpNode_pointer *numeric_conditions_lh, *numeric_conditions_rh;
  int num_numeric_conditions;

    int *numeric_conditions_lh_t;

  NumericEffectType *numeric_effects_neft;
  int *numeric_effects_fl;

  int *numeric_effects_fl_t;

  ExpNode_pointer *numeric_effects_rh;
  int num_numeric_effects;

  /* LNF
   */
  Comparator *lnf_conditions_comp;
  LnfExpNode_pointer *lnf_conditions_lh;

  int *lnf_conditions_lh_t;

  float *lnf_conditions_rh;
  int num_lnf_conditions;  

  NumericEffectType *lnf_effects_neft;
  int *lnf_effects_fl;

  int *lnf_effects_fl_t;

  LnfExpNode_pointer *lnf_effects_rh;
  int num_lnf_effects;

  /* this is true iff the numerical part of the effects affects or accesses
   * an undefined fluent (i.e. in numeric_effects_fl or numeric_effects_rh ) 
   * --- then, if the effect appears, the action is
   * illegal.
   */
  Bool illegal;

  /* helper
   */
  Bool removed;

  float cost;

} ActionEffect;



typedef struct _Action {

  NormOperator *norm_operator;
  PseudoAction *pseudo_action;

  char *name;
  int num_name_vars;
  int name_inst_table[MAX_VARS];

  int inst_table[MAX_VARS];

  int *preconds;
  int num_preconds;

  int *preconds_t;

  int *pref_preconds;
  int num_pref_preconds;

  float *timewindowsMin;
  int timewindowsMinindex;
  float *timewindowsMax;
  int timewindowsMaxindex;
  float timeMin;
  float timeMax; 
    
  TokenList_pointer *tw_pre;  
  int *tw_pre_t;

  TimeWindows *time_windows_list;
  TimeWindows_pointer *array_interval_list;
  int num_interval;
    /*-------duration--------*/

    ExpNode *action_duration;
    LnfExpNode *lnf_duration;

    /*------end-of-duration---*/

  /* numeric part, in general format, with fluents encoded as fl ints
   *
   * also, will (?) be transformed to lh fl, rh float; then, expnodes as
   * fast accessible as specialised structures. 
   */
  Comparator *numeric_preconds_comp;
  ExpNode_pointer *numeric_preconds_lh, *numeric_preconds_rh;

  int *numeric_preconds_lh_t;

  int num_numeric_preconds;

  /* LNF
   */
  Comparator *lnf_preconds_comp;
  LnfExpNode_pointer *lnf_preconds_lh;

    int *lnf_preconds_lh_t;

  float *lnf_preconds_rh;
  int num_lnf_preconds;

  ActionEffect *effects;
  int num_effects;

  struct _Action *next;

} Action;





/*****************************************************
 * structure for xml parser *
 *****************************************************/



typedef struct _XMLDependOnOp {

TokenList *name;

TypedList *depend_art;

struct _XMLDependOnOp *next;

} XMLDependOnOp;


typedef struct _XMLOperator {

TokenList *name;

XMLDependOnOp *depend_on_ops;

struct _XMLOperator *next;

} XMLOperator, *XMLOperator_pointer;



typedef struct _TokenListOperator {

 TokenList *name;
 float duration, timemin, timemax;

} TokenListOperator, *TokenListOperator_pointer;







/*****************************************************
 * BASIC OP AND FT STRUCTURES FOR CONNECTIVITY GRAPH *
 *****************************************************/











typedef struct _OpConn {

  /* to get name
   */
  Action *action;
  /* duration
   */
  float duration;  
  float timeMin;
  float timeMax;
  /* effects
   */
  int *E;
  int num_E;


  /* member for applicable actions extraction
   */
  Bool is_in_A;

  /* members for 1Ph - H(S) extraction
   */
  int is_used;
  Bool is_in_H;

  /* for time windows literals
  */
   TimeWindows *time_windows_list;
   int num_interval;

  /* to get the cost of the action
   */
  float cost;
} OpConn, *OpConn_pointer;



typedef struct _EfConn {

  int op;

  /* true if access to always undefined fluent, or
   * conflicting assignments.
   *
   * if that is the case then nothing except condition is set:
   * the effect is completely ignored except that
   * it renders the op unapplicable when its condition
   * is true.
   */
  Bool illegal;

  /* this one means we found in conn that it is useless (empty)
   */
  Bool removed;

  /* this is the cost; can be non-zero if a metric was specified
   * and established
   */
  float cost;

  int *PC;

    int *PC_t;

  int num_PC;
  /* numeric part
   */
  Comparator *f_PC_comp; /* either GEQ or GE */
  int *f_PC_fl;

    int *f_PC_fl_t;

  float *f_PC_c;
  int num_f_PC;
  /* array indexed by fl number, to fast know whether
   * new fluent value is high enough
   */
  Comparator *f_PC_direct_comp;
  float *f_PC_direct_c;

  /* logic effects
   */
  int *A;
  int num_A;
  int *D;
  int num_D;

  int *A_t;
  int *D_t;  

  /* and the numeric ones; fl_ is the encoding of the LNF
   * on the right hand side, without constant part
   * (special treatment for that as it's supposed
   *  to be the most common thing!!)
   */
  int *IN_fl;

    int *IN_fl_t;

    int *duration;

  int *IN_fl_;
  float *IN_c;
  int num_IN;

  int *AS_fl;

    int *AS_fl_t;

  int *AS_fl_;
  float *AS_c;
  int num_AS;

  /* implied effects
   */
  int *I;
  int num_I;

  /* members for relaxed fixpoint computation
   */
  int level;
  Bool in_E;
  int num_active_PCs;
  Bool ch;

  /* RPG
   */
  int num_active_f_PCs;

  /* 1P; an effect can be selected several times
   * for increasing a fluent.
   */
  int in_plan;

} EfConn;



typedef struct _FtConn {

  /* effects it is union conds, pres element of
   */
  int *PC;
  int num_PC;

  /* efs that add or del it
   */
  int *A;
  int num_A;

  int *D;
  int num_D;

  /* members for orderings preprocessing
   */
  int *False;
  int num_False;

  /* members for relaxed fixpoint computation
   */
  int level;
  Bool in_F;

 

  /* members for 1Ph extraction
   */
  int is_goal;
  int is_true;
  Bool ch;

  /* search
   */
  int rand;/* for hashing */

} FtConn;



typedef struct _FlConn {

  /* effects it is union conds, pres required
   */
  int *PC;
  int num_PC;

  /* efs that inc, ass it and by which encoded fluents and constants
   */
  int *IN;
  int *IN_fl_;
  float *IN_c;
  int num_IN;

  int *AS;
  int *AS_fl_;
  float *AS_c;/* see above */
  int num_AS;

  /* is it an artificial fluent?
   */
  Bool artificial;
  /* if so, then this here is the linear equation
   * it stands for
   */
  int *lnf_F;
  float *lnf_C;
  int num_lnf;


  /* the termination criterion for RPG building is based on mneed, see
   * JAIR article for definition;
   *
   * as the name suggests, we use the bool to indicate that this one is not
   * needed at all
   */
  Bool mneed_is_minusinfty;
  float mneed;
  /* see JAIR; shortcut for never needed at all.
   */
  Bool relevant;

  /* the following are members handled within heuristic algorithms.
   */

  /* this are arrays saying what the max value at 
   * the levels in the RPG is, resp. whether the value
   * can be defined there at all, resp. what the increasers
   * at that level have added.
   */
  Bool *def;
  float *level;

  /* for handling assigners in RPG: is an assigner in there yet,
   * and if so what is their max value?
   */
  Bool curr_assigned;
  float curr_max_assigned;

  int rand;/* for hashing */

} FlConn;




/* for multiple time windows literals
 */

typedef struct _OpConn_list {

    OpConn op1;
    int f;
  struct _OpConn_list *next;

} OpConn_list;






/****************************
 * STRUCTURES FOR SEARCHING *
 ****************************/









typedef struct _State {
  
  int *F;
  int num_F;

  Bool *f_D;
  float *f_V;
  int pred_index; /* For external plan reconstruction */ 
  int op;
  float cost;
  float stt;

} State, *State_pointer;



typedef struct _EhcNode {
  
  State S;

  int op;
  int depth;

  struct _EhcNode *father;
  struct _EhcNode *next;

} EhcNode;



typedef struct _EhcHashEntry {

  int sum;

  EhcNode *ehc_node;

  struct _EhcHashEntry *next;

} EhcHashEntry, *EhcHashEntry_pointer;



typedef struct _PlanHashEntry {

  int sum;
  State S;

  /* step is number of op that is EXECUTED in S;
   * -1 means that this state is no longer contained in plan
   */
  int step;
  struct _PlanHashEntry *next_step;

  struct _PlanHashEntry *next;

} PlanHashEntry, *PlanHashEntry_pointer;



typedef struct _BfsNode {
  
  State S;

  int op;
  int g;

  /* h is plain heuristic relaxed plan steps (needed for goal state recognition)
   * int_fn is optimized function value in plansteps case
   * float_fn is optimized function value in expression case
   */
  int h;
  int int_fn;
  float float_fn;

  int *H;
  int num_H;

  struct _BfsNode *father;

  struct _BfsNode *next;
  struct _BfsNode *prev;

} BfsNode;



typedef struct _BfsHashEntry {

  int sum;

  BfsNode *bfs_node;

  struct _BfsHashEntry *next;

} BfsHashEntry, *BfsHashEntry_pointer;













/*
 *  -------------------------------- MAIN FN HEADERS ----------------------------
 */

















void output_planner_info( void );
void ff_usage( void );
Bool process_command_line( int argc, char *argv[] );









/*
 *  ----------------------------- GLOBAL VARIABLES ----------------------------
 */












/*******************
 * GENERAL HELPERS *
 *******************/










/* used to time the different stages of the planner
 */
extern float gtempl_time, greach_time, grelev_time, gconn_time;
extern float gLNF_time, gsearch_time;
extern float gpert_time;

/* the command line inputs
 */
extern struct _command_line gcmd_line;

/* number of states that got heuristically evaluated
 */
extern int gevaluated_states;

/* maximal depth of breadth first search
 */
extern int gmax_search_depth;

extern Bool isfluents;

extern Bool isDurative;

extern Bool isTotalCost;
extern Bool isTempTimeWindows;

extern Bool isPreference;

extern Bool isDomainPreference;

extern Bool isConstraints;

extern Bool isConstraintsPreference;

extern WffNode *gconstraints;

extern WffNode *gconstraints_pref;

extern WffNode *gtil_constraints;

extern WffNode *gtil_constraints_pref;

extern WffNode *gwithin_constraints_pref;

extern WffNode *gwithin_constraints;

extern PlNode *within_constraints;

extern PlNode *within_constraints_pref;

extern XMLOperator *op_depend_op;

extern Bool breakForUser;

extern Bool graph_heuristic;

extern Bool sched_heuristic;

extern PlNode *pref_head;

extern WffNode *gpref_head;

extern PlNode *domain_pref_head;

extern PlNode *pref_constraints_head;

extern PlNode *pref_constraints_at_end;

extern PlNode *constraints_at_end;

extern PlNode *til_constraints;

extern PlNode *til_constraints_pref;

extern char automata_name[30];

extern float prune;

extern int *relevant_pref_preconds_predicate;
extern int gnum_pref_preconds_predicate;

extern TokenList_pointer  *relevant_pref_predicate;
extern int gnum_pref_predicate;

extern Fact_pointer *relevant_gpref_predicate;
extern int gnum_gpref_predicate;


/***********
 * PARSING *
 ***********/











/* used for pddl parsing, flex only allows global variables
 */
extern int gbracket_count;
extern char *gproblem_name;

/* The current input line number
 */
extern int lineno;

/* The current input filename
 */
extern char *gact_filename;

/* The pddl domain name
 */
extern char *gdomain_name;

/* loaded, uninstantiated operators
 */
extern PlOperator *gloaded_ops;
extern PlOperator *du_gloaded_ops;

/* stores initials as fact_list 
 */
extern PlNode *gorig_initial_facts;

extern Time_Ini_Literal *Timed_Initial_Literals;

/* not yet preprocessed goal facts
 */
extern PlNode *gorig_goal_facts;

/* not yet preprocessed constraints facts
 */
extern PlNode *gorig_constraints_facts;

/* not yet preprocessed constraints facts
 */
extern PlNode *gorig_domain_constraints_facts;

/* the types, as defined in the domain file
 */
extern TypedList *gparse_types;

/* the constants, as defined in domain file
 */
extern TypedList *gparse_constants;

/* the predicates and their arg types, as defined in the domain file
 */
extern TypedListList *gparse_predicates;

/* the functions and their arg types, as defined in the domain file
 */
extern TypedListList *gparse_functions;

/* the objects, declared in the problem file
 */
extern TypedList *gparse_objects;

/* the metric
 */
extern Token gparse_optimization;
extern ParseExpNode *gparse_metric;



/* connection to instantiation ( except ops, goal, initial )
 */

/* all typed objects 
 */
extern FactList *gorig_constant_list;

/* the predicates and their types
 */
extern FactList *gpredicates_and_types;

/* the functions and their types
 */
extern FactList *gfunctions_and_types;



/* for xml
 */
extern XMLOperator *op_depend_op;
extern XMLOperator_pointer *xml_operators;
extern TokenListOperator_pointer *instance_Operators;


extern TokenList_pointer *twpredicate;








/*****************
 * INSTANTIATING *
 *****************/










/* global arrays of constant names,
 *               type names (with their constants),
 *               predicate names,
 *               predicate aritys,
 *               defined types of predicate args
 */
extern Token gconstants[MAX_CONSTANTS];
extern int gnum_constants;
extern Token gtype_names[MAX_TYPES];
extern int gtype_consts[MAX_TYPES][MAX_TYPE];
extern Bool gis_member[MAX_CONSTANTS][MAX_TYPES];
extern int gtype_size[MAX_TYPES];
extern int gnum_types;
extern Token gpredicates[MAX_PREDICATES];
extern int garity[MAX_PREDICATES];
extern int gpredicates_args_type[MAX_PREDICATES][MAX_ARITY];
extern int gnum_predicates;
extern Token gfunctions[MAX_FUNCTIONS];
extern int gf_arity[MAX_FUNCTIONS];
extern int gfunctions_args_type[MAX_FUNCTIONS][MAX_ARITY];
extern int gnum_functions;




/* the domain in first step integer representation
 */
extern Operator_pointer goperators[MAX_OPERATORS];
extern int gnum_operators;
extern Fact *gfull_initial;
extern int gnum_full_initial;
extern FluentValue *gfull_fluents_initial;
extern int gnum_full_fluents_initial;
extern WffNode *ggoal;

extern ExpNode *gmetric;



/* stores inertia - information: is any occurence of the predicate
 * added / deleted in the uninstantiated ops ?
 */
extern Bool gis_added[MAX_PREDICATES];
extern Bool gis_deleted[MAX_PREDICATES];

/* for functions we *might* want to say, symmetrically, whether it is
 * increased resp. decreased at all.
 *
 * that is, however, somewhat involved because the right hand
 * sides can be arbirtray expressions, so we have no guarantee
 * that increasing really does adds to a functions value...
 *
 * thus (for the time being), we settle for "is the function changed at all?"
 */
extern Bool gis_changed[MAX_FUNCTIONS];



/* splitted initial state:
 * initial non static facts,
 * initial static facts, divided into predicates
 * (will be two dimensional array, allocated directly before need)
 */
extern Facts *ginitial;
extern int gnum_initial;
extern Fact **ginitial_predicate;
extern int *gnum_initial_predicate;

/* same thing for functions
 */
extern FluentValues *gf_initial;
extern int gnum_f_initial;
extern FluentValue **ginitial_function;
extern int *gnum_initial_function;



/* the type numbers corresponding to any unary inertia
 */
extern int gtype_to_predicate[MAX_PREDICATES];
extern int gpredicate_to_type[MAX_TYPES];

/* (ordered) numbers of types that new type is intersection of
 */
extern TypeArray gintersected_types[MAX_TYPES];
extern int gnum_intersected_types[MAX_TYPES];



/* splitted domain: hard n easy ops
 */
extern Operator_pointer *ghard_operators;
extern int gnum_hard_operators;
extern NormOperator_pointer *geasy_operators;
extern int gnum_easy_operators;



/* so called Templates for easy ops: possible inertia constrained
 * instantiation constants
 */
extern EasyTemplate *geasy_templates;
extern int gnum_easy_templates;



/* first step for hard ops: create mixed operators, with conjunctive
 * precondition and arbitrary effects
 */
extern MixedOperator *ghard_mixed_operators;
extern int gnum_hard_mixed_operators;



/* hard ''templates'' : pseudo actions
 */
extern PseudoAction_pointer *ghard_templates;
extern int gnum_hard_templates;



/* store the final "relevant facts"
 */
extern Fact grelevant_facts[MAX_RELEVANT_FACTS];
extern int gnum_relevant_facts;
extern int gnum_pp_facts;
/* store the "relevant fluents"
 */
extern Fluent grelevant_fluents[MAX_RELEVANT_FLUENTS];
extern int gnum_relevant_fluents;
extern Token grelevant_fluents_name[MAX_RELEVANT_FLUENTS];
/* this is NULL for normal, and the LNF for
 * artificial fluents.
 */
extern LnfExpNode_pointer grelevant_fluents_lnf[MAX_RELEVANT_FLUENTS];



/* the final actions and problem representation
 */
extern Action *gactions;
extern int gnum_actions;
extern State ginitial_state;
extern int *glogic_goal;
extern int gnum_logic_goal;
extern Comparator *gnumeric_goal_comp;
extern ExpNode_pointer *gnumeric_goal_lh, *gnumeric_goal_rh;
extern int gnum_numeric_goal;



/* to avoid memory leaks; too complicated to identify
 * the exact state of the action to throw away (during construction),
 * memory gain not worth the implementation effort.
 */
extern Action *gtrash_actions;



/* additional lnf step between finalized inst and
 * conn graph
 */
extern Comparator *glnf_goal_comp;
extern LnfExpNode_pointer *glnf_goal_lh;
extern float *glnf_goal_rh;
extern int gnum_lnf_goal;

extern LnfExpNode glnf_metric;
extern Bool goptimization_established;



/**********************
 * CONNECTIVITY GRAPH *
 **********************/





/* one ops (actions) array ...
 */
extern OpConn *gop_conn;
extern int gnum_op_conn;



/* one effects array ...
 */
extern EfConn *gef_conn;
extern int gnum_ef_conn;



/* one facts array.
 */
extern FtConn *gft_conn;
extern int gnum_ft_conn;



/* and: one fluents array.
 */
extern FlConn *gfl_conn;
extern int gnum_fl_conn;
extern int gnum_real_fl_conn;/* number of non-artificial ones */



/* final goal is also transformed one more step.
 */
extern int *gflogic_goal;
extern int gnum_flogic_goal;
extern Comparator *gfnumeric_goal_comp;
extern int *gfnumeric_goal_fl;
extern float *gfnumeric_goal_c;
extern int gnum_fnumeric_goal;

/* direct access (by relevant fluents)
 */
extern Comparator *gfnumeric_goal_direct_comp;
extern float *gfnumeric_goal_direct_c;













/*******************
 * SEARCHING NEEDS *
 *******************/












/* applicable actions
 */
extern int *gA;
extern int gnum_A;



/* communication from extract 1.P. to search engine:
 * 1P action choice
 */
extern int *gH;
extern int gnum_H;
/* cost of relaxed plan
 */
extern float gcost;



/* to store plan
 */
extern int gplan_ops[MAX_PLAN_LENGTH];
extern float gplan_ops_durations[MAX_PLAN_LENGTH];
extern int gnum_plan_ops;



/* stores the states that the current plan goes through
 */
extern State gplan_states[MAX_PLAN_LENGTH + 1];



/* dirty: multiplic. of total-time in final metric LNF
 */
extern float gtt;






/* the mneed structures
 *
 * assign propagation pairs i, j, and transitive such pairs.
 */
extern Bool **gassign_influence;
extern Bool **gTassign_influence;



/* the real var input to the mneed computation.
 */
extern Bool *gmneed_start_D;
extern float *gmneed_start_V;



/* does this contain conditional effects?
 * (if it does then the state hashing has to be made more
 *  cautiously)
 */
extern Bool gconditional_effects;



#endif