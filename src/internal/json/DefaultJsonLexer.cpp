#include "arxengine/internal/json/DefaultJsonLexer.h"
#include "arxengine/Json.h"

ARX_NAMESPACE_BEGIN

DefaultJsonLexerException::DefaultJsonLexerException(std::string_view reason) 
    : ArxException(ArxException::ErrorCode::DefaultJsonLexerFailure, reason)
{
}

std::vector<std::pair<LexerValueType, std::string>> DefaultJsonLexer::GetTokens(std::string_view text) 
{
    std::vector<std::pair<LexerValueType, std::string>> tokens;
    std::string token;
    std::stack<LexerValueType> valueHolders;

    StateMachine<void, LexerArgs> stateMachine { 
        LexerState::GetInitialBracket,
        {
            ObjFnWrapper(this, &DefaultJsonLexer::GetInitialBracketHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::GetIdentifierInitHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::GetIdentifierHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::GetOperatorHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::GetObjHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadStringHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadEscapeCharacterHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::GetCommaOrClosedBracketHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::FinishHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadIntHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadDoubleHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadNullHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadTrueHandler),
            ObjFnWrapper(this, &DefaultJsonLexer::ReadFalseHandler)
        }
    };

    LexerArgs args{tokens, token, valueHolders, 0};
    stateMachine.SetInstance(&args);

    for (size_t i = 0; i < text.size(); i++)
    {
#ifdef DEBUG_ARX_JSON_LEXER
        std::cout << "symbol " << text[i] << ", current state " << stateMachine.GetState() << std::endl;
#endif
        args.readLetter = text[i];
        stateMachine.Process();
    }

    if (stateMachine.GetState() != LexerState::Finish)
        throw DefaultJsonLexerException("Unexpected end of string");    

    return tokens;
}

bool DefaultJsonLexer::IsSkippable(char c) { return isblank(c) || c == '\n'; }

void DefaultJsonLexer::GetInitialBracketHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();
    instance->token += instance->readLetter;

    if (instance->readLetter == '{')
    {
        instance->valueHolders.push(LexerValueType::OpenedCurlyBracket);
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::OpenedCurlyBracket, GetIndentifierInit);
    }
    else if (instance->readLetter == '[')
    {
        instance->valueHolders.push(LexerValueType::OpenedSquareBracket);
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::OpenedCurlyBracket, GetObj);
    }
    else if (IsSkippable(instance->readLetter))
        return;
    else
        throw DefaultJsonLexerException(std::string("Expected '{' or '[' but got ") + instance->readLetter);
}

void DefaultJsonLexer::GetIdentifierInitHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == '"')
        stateMachine.SetState(LexerState::GetIdentifier);
    else if (instance->readLetter == '}')
    {
        instance->tokens.push_back(std::make_pair(LexerValueType::ClosedCurlyBracket, std::string("}")));
        stateMachine.SetState(LexerState::Finish);
    }
    else if (!IsSkippable(instance->readLetter))
        throw DefaultJsonLexerException(std::string("Expected '\"' but got ") + instance->readLetter);
}

void DefaultJsonLexer::GetIdentifierHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == '"')
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::Identifier, LexerState::GetOperator);
    else if (isalnum(instance->readLetter) || instance->readLetter == '_')
        instance->token += instance->readLetter ;
    else
        throw DefaultJsonLexerException(std::string("Expected character/number but got ") + instance->readLetter);
}

void DefaultJsonLexer::GetOperatorHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == ':')
        stateMachine.SetState(LexerState::GetObj);
    else if (!IsSkippable(instance->readLetter))
        throw DefaultJsonLexerException(std::string("expected ':' but got ") + instance->readLetter);
}

void DefaultJsonLexer::GetObjHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();
    
    if (instance->readLetter == '{')
    {
        instance->token += instance->readLetter;
        instance->valueHolders.push(LexerValueType::OpenedCurlyBracket);
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::OpenedCurlyBracket, LexerState::GetIndentifierInit);
    }               
    else if (instance->readLetter == '[')
    {
        instance->token += instance->readLetter;
        instance->valueHolders.push(LexerValueType::OpenedSquareBracket);
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::OpenedSquareBracket, LexerState::GetObj);
    }
    else if (instance->readLetter == ']')
    {
        instance->token += instance->readLetter;
        GetCommaOrClosedBracketHandler(stateMachine);
    }
    else if (instance->readLetter == '"')
        stateMachine.SetState(LexerState::ReadString);
    else if (instance->readLetter == 't')
    {
        instance->token += instance->readLetter;
        stateMachine.SetState(LexerState::ReadTrue);
    }
    else if (instance->readLetter == 'f')
    {
        instance->token += instance->readLetter;
        stateMachine.SetState(LexerState::ReadFalse);
    }
    else if (instance->readLetter == 'n')
    {
        instance->token += instance->readLetter;
        stateMachine.SetState(LexerState::ReadNull);
    }
    else if (isdigit(instance->readLetter) || instance->readLetter == '-')
    {
        instance->token += instance->readLetter;
        stateMachine.SetState(LexerState::ReadInt);
    }
    else if (!IsSkippable(instance->readLetter))
        throw DefaultJsonLexerException(std::string("Unexpected character ") + instance->readLetter);
}

void DefaultJsonLexer::ReadStringHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == '"')
    {
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::String, LexerState::GetCommaOrClosedBracket);
        return;
    }
    else if (instance->readLetter == '\\')
    {
        stateMachine.SetState(LexerState::ReadEscapeCharacter);
        return;
    }

    instance->token += instance->readLetter;
}

void DefaultJsonLexer::ReadEscapeCharacterHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();
    const char characters[12]{ 'n', 't', 'b', 'r', 'a', '\'', '"', '?', '\\', 'f', 'v', '\0'};
    const char escapeCharacters[12]{ '\n', '\t', '\b', '\r', '\a', '\'', '\"', '\?', '\\', '\f', '\v', '\0'};

    for (size_t i = 0; i < (sizeof(characters) / sizeof(char)); i++)
    {
        if (instance->readLetter == characters[i])
        {
            instance->token += escapeCharacters[i];
            stateMachine.SetState(LexerState::ReadString);
            return;
        }
    }

    /*if not found*/
    throw DefaultJsonLexerException("Invalid escape character");
}

void DefaultJsonLexer::GetCommaOrClosedBracketHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == ',')
    {
        instance->token.clear();
        if (instance->valueHolders.top() == LexerValueType::OpenedCurlyBracket)
            stateMachine.SetState(LexerState::GetIndentifierInit);
        else
            stateMachine.SetState(LexerState::GetObj);
    }
    else if (instance->readLetter == '}')
    {
        instance->token += instance->readLetter;
        if (instance->valueHolders.top() != LexerValueType::OpenedCurlyBracket)
           throw DefaultJsonLexerException("Brackets mismatch, expected ']'");

        instance->valueHolders.pop();
        if (instance->valueHolders.size() == 0)
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedCurlyBracket, LexerState::Finish);
        else if (instance->valueHolders.top() == LexerValueType::OpenedCurlyBracket)
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedCurlyBracket, LexerState::GetCommaOrClosedBracket);
        else
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedCurlyBracket, LexerState::GetObj);
                                
    }
    else if (instance->readLetter == ']')
    {
        instance->token += instance->readLetter;
        if (instance->valueHolders.top() != LexerValueType::OpenedSquareBracket)
            throw DefaultJsonLexerException("Brackets mismatch, expected '}'");

        instance->valueHolders.pop();
        if (instance->valueHolders.size() == 0)
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedSquareBracket, LexerState::Finish);
        else if (instance->valueHolders.top() == LexerValueType::OpenedCurlyBracket)
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedSquareBracket, LexerState::GetCommaOrClosedBracket);
        else
            StoreAndChangeStateMachineState(stateMachine, LexerValueType::ClosedSquareBracket, LexerState::GetObj);
    }
    else if (!IsSkippable(instance->readLetter))
        throw DefaultJsonLexerException(std::string("Unexpected character ") + instance->readLetter);
}

void DefaultJsonLexer::FinishHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (!IsSkippable(instance->readLetter))
        throw DefaultJsonLexerException("Unexpected symbols after main object/array end");
}

void DefaultJsonLexer::ReadIntHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (instance->readLetter == '.' && instance->token != "-") 
    {
        instance->token += '.';
        stateMachine.SetState(LexerState::ReadDouble);
    }
    else if (IsSkippable(instance->readLetter) && instance->token != "-")
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::Number, LexerState::GetCommaOrClosedBracket);
    else if ((instance->readLetter == ']' || instance->readLetter == '}' || instance->readLetter == ',') && instance->token != "-")
    {
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::Number, LexerState::GetCommaOrClosedBracket);
        GetCommaOrClosedBracketHandler(stateMachine);
    }
    else if (isdigit(instance->readLetter))
        instance->token += instance->readLetter;
    else if (instance->token == "-")
        throw DefaultJsonLexerException("Expected digit after minus sign");
    else
        throw DefaultJsonLexerException(std::string("Expected digit but got ") + instance->readLetter);
}

void DefaultJsonLexer::ReadDoubleHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();

    if (IsSkippable(instance->readLetter))
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::FloatNumber, LexerState::GetCommaOrClosedBracket);
    else if (instance->readLetter == ']' || instance->readLetter == '}' || instance->readLetter == ',')
    {
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::FloatNumber, LexerState::GetCommaOrClosedBracket);
        GetCommaOrClosedBracketHandler(stateMachine);
    }
    else if (!isdigit(instance->readLetter))
        throw DefaultJsonLexerException(std::string("Expected digit but got ") + instance->readLetter);

    instance->token += instance->readLetter;
}

void DefaultJsonLexer::ReadNullHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    auto *instance = stateMachine.GetInstance();
    const char *nullStr = "null";
    instance->token += instance->readLetter;
    size_t pos = instance->token.length() - 1;

    if (instance->token[pos] != nullStr[pos])
        throw DefaultJsonLexerException(std::string("(NULLOBJ) Expected ") + nullStr[pos] + " but got " + instance->token[pos]);
    else if (instance->token.length() == 4)
         StoreAndChangeStateMachineState(stateMachine, LexerValueType::Null, GetCommaOrClosedBracket);
}

void DefaultJsonLexer::ReadTrueHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    const char *trueStr = "true";
    auto *instance = stateMachine.GetInstance();
    instance->token += instance->readLetter;
    size_t pos = instance->token.length() - 1;

    if (instance->token[pos] != trueStr[pos])
        throw DefaultJsonLexerException("(BOOL) Expected true");
    else if (instance->token.length() == 4)
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::Bool, LexerState::GetCommaOrClosedBracket);
}

void DefaultJsonLexer::ReadFalseHandler(StateMachine<void, LexerArgs> &stateMachine)
{
    const char *falseStr = "false";
    auto *instance = stateMachine.GetInstance();
    instance->token += instance->readLetter;
    size_t pos = instance->token.length() - 1;

    if (instance->token[pos] != falseStr[pos])
        throw DefaultJsonLexerException("(BOOL) Expected false");
    else if (instance->token.length() == 5)
        StoreAndChangeStateMachineState(stateMachine, LexerValueType::Bool, LexerState::GetCommaOrClosedBracket);
}

void DefaultJsonLexer::StoreAndChangeStateMachineState(StateMachine<void, LexerArgs> &stateMachine, LexerValueType type, LexerState nextState)
{
    auto *instance = stateMachine.GetInstance();
    instance->tokens.push_back(std::make_pair(type, instance->token));
    instance->token.clear();
    stateMachine.SetState(nextState);
}

ARX_NAMESPACE_END
