/******************************************************************************
*
*   Extensions for engine and game
*
******************************************************************************/

#define NEIGHBORS_COUNT 9

typedef int RuleAttrib[NEIGHBORS_COUNT]; //zero index is the root(centre) cell

typedef struct Rule {
    RuleAttrib attribs;
    void* Behave;
};

extern RuleAttrib stay_rule, birth_rule;