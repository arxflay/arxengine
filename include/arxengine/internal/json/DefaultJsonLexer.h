#ifndef ARX_DEFAULTJSONLEXER_H
#define ARX_DEFAULTJSONLEXER_H
#include "arxengine/Json.h"
#include <stack>
#include "arxengine/internal/StateMachine.h"
ARX_NAMESPACE_BEGIN

class DefaultJsonLexerException : public ArxException
{
public:
    DefaultJsonLexerException (std::string_view reason);
};

class DefaultJsonLexer : public IJsonLexer
{
public:
    enum LexerState
    {
        GetInitialBracket, // { or [
        GetIndentifierInit, // first occurence of "
        GetIdentifier, //accepts spaces and alpha symbols until finds another "
        GetOperator, // :
        GetObj, //determines type of object by first letter
        ReadString, //read string
        ReadEscapeCharacter, //read escape characters. only one character escape sequences are supported
        GetCommaOrClosedBracket, //expands array/object or finishes object/array (] or })
        Finish, //desired state
        ReadInt, //read int
        ReadDouble, //read double
        ReadNull, //read null, case sensetive
        ReadTrue, //read true, case sensetive
        ReadFalse //read false, case sensetive
    };
    
    struct LexerArgs
    {
        std::vector<std::pair<LexerValueType, std::string>> &tokens;
        std::string &token;
        std::stack<LexerValueType> &valueHolders;
        char readLetter;
    };
    
    /*
     * @brief json string tokenization  
     * @description whitespaces are ignored
     * Token is a pair of token type and stringified value
     * @returns vector of tokens
     */
    std::vector<std::pair<LexerValueType, std::string>> GetTokens(std::string_view text) override;
    
private:
    bool IsSkippable(char c); 
    
    /*
     * @brief looks for first '[' or '{' in document
     */
    void GetInitialBracketHandler(StateMachine<void, LexerArgs> &stateMachine);
    
    /*
     * @brief looks for begin quotation '"' of identifier 
     */
    void GetIdentifierInitHandler(StateMachine<void, LexerArgs> &stateMachine);
    
    /*
     * @brief looks for end quotation '"' of identifier 
     */
    void GetIdentifierHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief looks for ':'
     */
    void GetOperatorHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief looks for end quotation '' of identifier 
     */
    void GetObjHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads string until '"' 
     */
    void ReadStringHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads escape character until the end of escape character
     * @description only limited amount of escape characters are supported
     */
    void ReadEscapeCharacterHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads ',' or closed bracket ('}' or ']')
     */
    void GetCommaOrClosedBracketHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads file until the end of file
     * @description throws if non whitespace character is found
     */
    void FinishHandler(StateMachine<void, LexerArgs> &stateMachine);
    
    /*
     * @brief reads int until non digit characters is found
     */
    void ReadIntHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads double until non digit and non dot characters is found
     */
    void ReadDoubleHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads null until all characters are valid     
     */
    void ReadNullHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads true until all characters are valid     
     */
    void ReadTrueHandler(StateMachine<void, LexerArgs> &stateMachine);

    /*
     * @brief reads false until all characters are valid     
     */
    void ReadFalseHandler(StateMachine<void, LexerArgs> &stateMachine);

    /**
     * @brief StoreAndChangeStateMachineState value stored in StateMachine instance, clears value and sets nextState
     * 
     * @param stateMachine State machine with LexerArgs
     * @param type type of value that is stored
     * @param nextState next StateMachine state
     */
    void StoreAndChangeStateMachineState(StateMachine<void, LexerArgs> &stateMachine, LexerValueType type, LexerState nextState);
};

ARX_NAMESPACE_END

#endif
