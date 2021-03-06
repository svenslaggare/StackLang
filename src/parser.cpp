#include <stdexcept>
#include "parser.h"
#include "lexer.h"
#include "ast/asts.h"
#include "typename.h"
#include "object.h"

Parser::Parser(const OperatorContainer& operators, std::vector<Token> tokens)
	: operators(operators), tokens(tokens), tokenIndex(-1) {

}

void Parser::error(std::string message) {
	throw std::runtime_error(std::to_string(mCurrentLineNumber) + ": " + message);
}

Token& Parser::nextToken() {
	tokenIndex++;

	if (tokenIndex >= tokens.size()) {
		error("Reached end of tokens.");
	}

	currentToken = tokens[tokenIndex];

	//Skip line breaks
	if (currentToken.type() != TokenType::LineBreak) {
		return currentToken;
	} else {
		mCurrentLineNumber++;
		return nextToken();
	}
}

Token& Parser::peekToken(int delta) {
	int nextTokenIndex = tokenIndex + delta;

	if (nextTokenIndex >= tokens.size()) {
		error("Reached end of tokens.");
	}

	//Skip line breaks
	if (currentToken.type() != TokenType::LineBreak) {
		return tokens[nextTokenIndex];
	} else {
		return peekToken(delta + 1);
	}
}

char Parser::currentTokenAsChar(std::string errorMessage) {
	if (currentToken.type() != TokenType::SingleChar) {
		error(errorMessage);
	}

	return currentToken.charValue;
}

bool Parser::isSingleCharToken(char character) {
	return currentToken.type() == TokenType::SingleChar && currentToken.charValue == character;
}

void Parser::assertCurrentTokenAsChar(char character, std::string errorMessage) {
	if (!isSingleCharToken(character)) {
		error(errorMessage);
	}
}

int Parser::getTokenPrecedence() {
	if (!(currentToken.type() == TokenType::SingleChar || currentToken.type() == TokenType::TwoChars)) {
		return -1;
	}

	if (currentToken.type() == TokenType::TwoChars) {
		Operator op(currentToken.charValue, currentToken.charValue2);

		if (operators.isBinaryDefined(op)) {
			return operators.getPrecedence(op);
		}
	} else {
		Operator op(currentToken.charValue);

		if (operators.isBinaryDefined(op)) {
			return operators.getPrecedence(op);
		}
	}

	return -1;
}

std::string Parser::parseNamespaceName() {
	std::string namespaceName;

	while (true) {
		if (currentToken.type() == TokenType::TwoChars
		&& currentToken.charValue == ':'
		&& currentToken.charValue2 == ':') {
			nextToken(); //Eat the '::'
			if (currentToken.type() != TokenType::Identifier) {
				error("Expected identifier after '::' in type name.");
			}

			namespaceName += "::" + currentToken.strValue;
			nextToken(); //Eat the identifier
		} else {
			break;
		}
	}

	return namespaceName;
}

std::string Parser::parseTypeName(bool allowArray) {
	std::string typeName = currentToken.strValue;

	nextToken(); //Eat the identifier

	//Parse namespace
	typeName += parseNamespaceName();

	if (allowArray) {
		//Check if array type
		while (isSingleCharToken('[')) {
			nextToken(); //Eat the '['
			assertCurrentTokenAsChar(']', "Expected ']'");
			nextToken(); //Eat the ']'

			typeName += "[]";
		}
	}

	return typeName;
}

std::shared_ptr<ExpressionAST> Parser::parseIntegerExpression() {
	auto intAst = std::make_shared<IntegerExpressionAST>(currentToken.intValue);
	nextToken(); //Consume the int
	return intAst;
}

std::shared_ptr<ExpressionAST> Parser::parseBoolExpression() {
	auto boolAst = std::make_shared<BoolExpressionAST>(currentToken.type() == TokenType::True);
	nextToken(); //Consume the bool value
	return boolAst;
}

std::shared_ptr<ExpressionAST> Parser::parseFloatExpression() {
	auto floatAst = std::make_shared<FloatExpressionAST>(currentToken.floatValue);
	nextToken(); //Consume the float
	return floatAst;
}

std::shared_ptr<ExpressionAST> Parser::parseNullRefExpression() {
	auto nullAst = std::make_shared<NullRefExpressionAST>();
	nextToken(); //Consume the null
	return nullAst;
}

std::shared_ptr<ExpressionAST> Parser::parseCastExpression() {
	//Eat the 'cast'
	nextToken();

	if (!isSingleCharToken('<')) {
		error("Expected '<' after cast operator.");
	}

	nextToken(); //Eat the '<'

	//Get the type name
	auto typeName = parseTypeName();

	if (!isSingleCharToken('>')) {
		error("Expected '>' after type name.");
	}

	nextToken(); //Eat the '<'

	if (!isSingleCharToken('(')) {
		error("Expected '('.");
	}

	nextToken(); //Eat the ')'

	auto expression = parseExpression();

	if (expression == nullptr) {
		return nullptr;
	} 

	if (!isSingleCharToken(')')) {
		error("Expected ')'.");
	}

	nextToken(); //Eat the ')'

	return std::make_shared<CastExpressionAST>(typeName, expression);
}

std::shared_ptr<ExpressionAST> Parser::parseCharExpression() {
	char value = currentToken.charValue;

	//Eat the char
	nextToken();

	return std::make_shared<CharExpressionAST>(value);
}

std::shared_ptr<ExpressionAST> Parser::parseStringExpression() {
	auto str = currentToken.strValue;

	//Eat the string
	nextToken();

	return std::make_shared<StringExpressionAST>(str);
}

std::shared_ptr<ExpressionAST> Parser::parseIdentifierExpression(bool allowDeclaration) {
	std::string identifier = currentToken.strValue;

	//Eat the identifier.
	nextToken();

	std::shared_ptr<ExpressionAST> identExpr = nullptr;

	//Parse namespace
	while (true) {
		if (currentToken.type() == TokenType::TwoChars
		&& currentToken.charValue == ':'
		&& currentToken.charValue2 == ':') {
			nextToken(); //Eat the '::'
			if (currentToken.type() != TokenType::Identifier) {
				error("Expected identifier after '::' in type name.");
			}

			identifier += "::" + currentToken.strValue;
			nextToken(); //Eat the identifier
		} else {
			break;
		}
	}

	//Variable decleration/reference
	if (!isSingleCharToken('(')) {
		//Check if array type
		if (isSingleCharToken('[')) {
			nextToken(); //Eat the '['

			//If not ']', its an array access
			std::shared_ptr<ExpressionAST> accessExpression = nullptr;
			if (!isSingleCharToken(']')) {
				accessExpression = parseExpression();
			}

			assertCurrentTokenAsChar(']', "Expected ']'");
			nextToken(); //Eat the ']'
	
			if (accessExpression != nullptr) {
				std::shared_ptr<ExpressionAST> refExpression = std::make_shared<VariableReferenceExpressionAST>(identifier);
				std::shared_ptr<ExpressionAST> arrayAccess = std::make_shared<ArrayAccessAST>(refExpression, accessExpression);

				while (isSingleCharToken('[')) {
					nextToken(); //Eat the '['
					accessExpression = parseExpression();

					if (accessExpression == nullptr) {
						return nullptr;
					}

					arrayAccess = std::make_shared<ArrayAccessAST>(arrayAccess, accessExpression);

					assertCurrentTokenAsChar(']', "Expected ']'");
					nextToken(); //Eat the ']'
				}

				identExpr = arrayAccess;
			} else {
				//Array type
				identifier += "[]";

				while (isSingleCharToken('[')) {
					nextToken(); //Eat the '['
					assertCurrentTokenAsChar(']', "Expected ']'");
					nextToken(); //Eat the ']'

					identifier += "[]";
				}
			}
		}

		if (identExpr == nullptr) {
			if (currentToken.type() == TokenType::Identifier) {
				if (!allowDeclaration) {
					error("Declaration is not allowed in the current expression.");
				}

				std::string varName = currentToken.strValue;
				nextToken(); //Eat the identifier

				//Declaration
				identExpr = std::make_shared<VariableDeclarationExpressionAST>(
					identifier,
					varName);
			} else {
				//Reference
				identExpr = std::make_shared<VariableReferenceExpressionAST>(identifier);
			}
		}
	}

	if (identExpr != nullptr) {
		return identExpr;
	}

	//Function call
	nextToken(); //Eat the '('
	std::vector<std::shared_ptr<ExpressionAST>> arguments;

	if (!(currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')')) {
		while (true) {
			auto arg = parseExpression();

			if (arg == nullptr) {
				return arg;
			}

			arguments.push_back(arg);

			if (isSingleCharToken(')')) {
				break;
			}

			if (!isSingleCharToken(',')) {
				error("Expected ',' or ')' in argument list.");
			}

			nextToken();
		}
	}

	//Eat the ')'
	nextToken();

	return std::make_shared<CallExpressionAST>(CallExpressionAST(identifier, arguments));
}

std::shared_ptr<ExpressionAST> Parser::parseNewExpression() {
	//Eat the 'new'
	nextToken();

	//Get the type name
	if (currentToken.type() != TokenType::Identifier) {
		error("Expected identifier.");
	}

	// auto typeName = currentToken.strValue;
	// nextToken(); //Eat the type name
	auto typeName = parseTypeName(false);
	
	if (isSingleCharToken('[')) {
		return parseNewArrayExpression(typeName);
	} else if (isSingleCharToken('(')) {
		return parseNewObjectExpression(typeName);
	} else {
		error("Expected '[' or ')'.");
		return nullptr;
	}
}

std::shared_ptr<ExpressionAST> Parser::parseNewObjectExpression(std::string typeName) {
	nextToken(); //Eat the '('
	std::vector<std::shared_ptr<ExpressionAST>> constructorArguments;

	if (!(currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')')) {
		while (true) {
			auto arg = parseExpression();

			if (arg == nullptr) {
				return arg;
			}

			constructorArguments.push_back(arg);

			if (isSingleCharToken(')')) {
				break;
			}

			if (!isSingleCharToken(',')) {
				error("Expected ',' or ')' in argument list.");
			}

			nextToken();
		}
	}

	//Eat the ')'
	nextToken();

	return std::make_shared<NewClassExpressionAST>(typeName, constructorArguments);
}

std::shared_ptr<ExpressionAST> Parser::parseNewArrayExpression(std::string elementTypeName) {
	nextToken(); //Eat the '['
	
	// //Get the length expression
	// auto lengthExpression = parseExpression();

	// if (lengthExpression == nullptr) {
	// 	return nullptr;
	// }

	// assertCurrentTokenAsChar(']', "Expected ']'.");
	// nextToken(); //Eat the ']'
	
	//Get the length expressions
	std::vector<std::shared_ptr<ExpressionAST>> lengthExpressions;

	while (true) {
		auto lengthExpression = parseExpression();

		if (lengthExpression == nullptr) {
			return nullptr;
		}

		lengthExpressions.push_back(lengthExpression);

		if (isSingleCharToken(',')) {
			nextToken(); //Eat the ','
		} else if (isSingleCharToken(']')) {
			nextToken(); //Eat the ']'
			break;
		} else {
			assertCurrentTokenAsChar(']', "Expected ']'.");
		}
	}

	//Parse trailing array types
	while (isSingleCharToken('[')) {
		nextToken(); //Eat the '['
		assertCurrentTokenAsChar(']', "Expected ']'");
		nextToken(); //Eat the ']'

		elementTypeName += "[]";
	}

	if (lengthExpressions.size() == 1) {
		return std::make_shared<ArrayDeclarationAST>(elementTypeName, lengthExpressions.at(0));
	} else {
		return std::make_shared<MultiDimArrayDeclarationAST>(elementTypeName, lengthExpressions);
	}
}

std::shared_ptr<ExpressionAST> Parser::parseParenthesisExpression() {
	nextToken(); //Eat the '('

	auto expr = parseExpression();

	if (expr == nullptr) {
		return expr;
	}

	assertCurrentTokenAsChar(')', "Expected ')'.");

	nextToken(); //Eat the ')'

	return expr;
}

std::shared_ptr<ExpressionAST> Parser::parseUsingNamespaceExpression() {
	nextToken(); //Eat the 'using'

	if (currentToken.type() != TokenType::Namespace) {
		error("Expected 'namespace' after using.");
	}

	nextToken(); //Eat the 'namespace'

	//Parse the name
	std::string namespaceName = currentToken.strValue;
	nextToken();
	namespaceName += parseNamespaceName();

	return std::make_shared<UsingNamespaceExpressionAST>(namespaceName);
}

std::shared_ptr<ExpressionAST> Parser::parsePrimaryExpression(bool allowDeclaration) {
	switch (currentToken.type()) {
	case TokenType::Integer:
		return parseIntegerExpression();
	case TokenType::True:
		return parseBoolExpression();
	case TokenType::False:
		return parseBoolExpression();	
	case TokenType::Float:
		return parseFloatExpression();
	case TokenType::Null:
		return parseNullRefExpression();	
	case TokenType::Char:
		return parseCharExpression();
	case TokenType::String:
		return parseStringExpression();
	case TokenType::Identifier:
		return parseIdentifierExpression(allowDeclaration);
	case TokenType::SingleChar:
		if (currentToken.charValue == '(') {
			return parseParenthesisExpression();
		}
		break;
	case TokenType::New:
		return parseNewExpression();
	case TokenType::Cast:
		return parseCastExpression();
	case TokenType::Using:
		return parseUsingNamespaceExpression();
	default:
		break;
	}	

	return std::shared_ptr<ExpressionAST>();
}

std::shared_ptr<ExpressionAST> Parser::parseBinaryOpRHS(int exprPrecedence, std::shared_ptr<ExpressionAST> lhs, bool allowEqualAssign) {
	while (true) {
		//If this is a bin op, find its precedence
		int tokPrec = getTokenPrecedence();

		//If this is a bin op  that binds as least as tightly as the current bin op, consume it, otherwise we are done.
		if (tokPrec < exprPrecedence) {
			return lhs;
		}

		Operator op = currentToken.type() == TokenType::SingleChar ?
			Operator(currentToken.charValue) :
			Operator(currentToken.charValue, currentToken.charValue2);

		if (!op.isTwoChars()) {
			if (!allowEqualAssign && op.op1() == '=') {
				error("Operator '=' not allowed in current expression.");
			}
		} else {
			if (!allowEqualAssign && operators.assignmentOperators().count(op.op1()) > 0 && op.op2() == '=') {
				error("Operator '=' not allowed in current expression.");
			}
		}

		nextToken(); //Eat the operator

		//Parse the unary expression after the binary operator
		auto rhs = parseUnaryExpression();

		if (rhs == nullptr) {
			return rhs;
		}

		//If binOp binds less tightly with RHS than the operator after RHS, let the pending operator take RHS as its LHS
		int nextPrec = getTokenPrecedence();
		if (tokPrec < nextPrec) {
			rhs = parseBinaryOpRHS(tokPrec + 1, rhs, allowEqualAssign);

			if (rhs == nullptr) {
				return rhs;
			}
		}

		//Merge LHS/RHS
		lhs = std::make_shared<BinaryOpExpressionAST>(lhs, rhs, op);
	}
}

std::shared_ptr<ExpressionAST> Parser::parseUnaryExpression(bool allowDeclaration) {
	//If the current token isn't an operator, is must be a primary expression
	if (currentToken.type() != TokenType::SingleChar || (isSingleCharToken('(') || isSingleCharToken(','))) {
		return parsePrimaryExpression(allowDeclaration);
	}

	//If this is a unary operator, read it.
	int opChar = currentToken.charValue;
	nextToken(); //Eat the operator

	auto operand = parseUnaryExpression(allowDeclaration);

	if (operand != nullptr) {
		auto op = Operator(opChar);
		if (!operators.isUnaryDefined(op)) {
			error("'" + op.asString() + "' is not a defined unary operator.");
		}

		return std::make_shared<UnaryOpExpressionAST>(operand, op);
	} 

	return operand;
}

std::shared_ptr<ExpressionAST> Parser::parseExpression(bool allowEqualAssign) {
	auto lhs = parseUnaryExpression(allowEqualAssign);

	if (lhs == nullptr) {
		return lhs;
	}

	return parseBinaryOpRHS(0, lhs, allowEqualAssign);
}

std::shared_ptr<StatementAST> Parser::parseIfElseStatement() {
	nextToken(); //Eat the 'if'

	assertCurrentTokenAsChar('(', "Expected '('.");
	nextToken(); //Eat the '('

	auto condExpr = parseExpression();

	assertCurrentTokenAsChar(')', "Expected ')'.");
	nextToken(); //Eat the ')'

	//Parse the then body
	auto thenBody = parseBlock();

	//Check if else
	//nextToken();
	std::shared_ptr<BlockAST> elseBody = nullptr;

	if (peekToken().type() == TokenType::Else) {
		nextToken(); //Eat the 'else'.

		nextToken(); 
		elseBody = parseBlock();
	}

	return std::make_shared<IfElseStatementAST>(condExpr, thenBody, elseBody);
}

std::shared_ptr<StatementAST> Parser::parseWhileLoopStatement() {
	nextToken(); //Eat the 'while'

	assertCurrentTokenAsChar('(', "Expected '('.");
	nextToken(); //Eat the '('

	auto condExpr = parseExpression();
	assertCurrentTokenAsChar(')', "Expected ')'.");
	nextToken(); //Eat the ')'

	//Parse the body
	auto bodyBlock = parseBlock();

	return std::make_shared<WhileLoopStatementAST>(condExpr, bodyBlock);
}

std::shared_ptr<StatementAST> Parser::parseForLoopStatement() {
	nextToken(); //Eat the 'for'

	assertCurrentTokenAsChar('(', "Expected '('.");
	nextToken(); //Eat the '('

	auto initExpr = parseExpression(true);
	assertCurrentTokenAsChar(';', "Expected ';' after initialization in for loop..");
	nextToken(); //Eat the ';'

	auto condExpr = parseExpression();
	assertCurrentTokenAsChar(';', "Expected ';' after condition in for loop.");
	nextToken(); //Eat the ';'

	auto changeExpr = parseExpression(true);

	assertCurrentTokenAsChar(')', "Expected ')'.");
	nextToken(); //Eat the ')'

	//Parse the body
	auto bodyBlock = parseBlock();

	return std::make_shared<ForLoopStatementAST>(initExpr, condExpr, changeExpr, bodyBlock);
}

std::shared_ptr<StatementAST> Parser::parseStatement() {
	std::shared_ptr<StatementAST> statement;

	switch (currentToken.type()) {
	case TokenType::Return:
		{
			nextToken(); //Eat the 'return'

			if (isSingleCharToken(';')) {
				statement = std::make_shared<ReturnStatementAST>();
			} else {
				auto returnExpr = parseExpression();
				statement = std::make_shared<ReturnStatementAST>(returnExpr);
				assertCurrentTokenAsChar(';', "Expected ';' after statement.");
			}
		}
		break;
	case TokenType::If:
		statement = parseIfElseStatement();
		break;
	case TokenType::While:
		statement = parseWhileLoopStatement();
		break;
	case TokenType::For:
		statement = parseForLoopStatement();
		break;
	case TokenType::SingleChar:
		if (isSingleCharToken('{')) {
			statement = parseBlock();
			break;
		}
	default:
		//Simple statement, one expression.
		auto expr = parseExpression(true);
		statement = std::make_shared<ExpressionStatementAST>(expr);
		assertCurrentTokenAsChar(';', "Expected ';' after statement.");
		break;
	}

	return statement;
}

std::shared_ptr<BlockAST> Parser::parseBlock() {
	assertCurrentTokenAsChar('{', "Expected '{'.");

	std::vector<std::shared_ptr<StatementAST>> statements;

	nextToken(); //Eat the '{'

	while (true) {
		if (isSingleCharToken('}')) {
			break;
		}

		auto statement = parseStatement();

		if (statement != nullptr) {
			statements.push_back(statement);
		}

		nextToken();
	}

	return std::make_shared<BlockAST>(statements);
}

std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> Parser::parseFunctionParameters() {
	nextToken(); //Eat the '('

	std::vector<std::shared_ptr<VariableDeclarationExpressionAST>> parameters;
	if (!(currentToken.type() == TokenType::SingleChar && currentToken.charValue == ')')) {
		while (true) {
			if (currentToken.type() == TokenType::Identifier) {
				std::string varType = parseTypeName();

				if (currentToken.type() == TokenType::Identifier) {
					parameters.push_back(std::make_shared<VariableDeclarationExpressionAST>(
						varType,
						currentToken.strValue,
						true));
				}
			}

			nextToken();

			if (isSingleCharToken(')')) {
				break;
			}

			if (!isSingleCharToken(',')) {
				error("Expected ',' or ')' in argument list.");
			}

			nextToken();
		}
	}

	nextToken(); //Eat the ')'
	return parameters;
}

std::shared_ptr<FunctionPrototypeAST> Parser::parseFunctionPrototype() {
	//Eat the 'func'
	nextToken();

	//The name
	if (currentToken.type() != TokenType::Identifier) {
		error("Expected identifier.");
	}

	std::string name = currentToken.strValue;

	//Parameters
	nextToken(); //Eat the name
	if (currentTokenAsChar() != '(')  {
		error("Expected ')' in prototype.");
	}

	auto parameters = parseFunctionParameters();

	//Return type
	if (currentTokenAsChar() != ':')  {
		error("Expected ':' after parameters.");
	}

	nextToken();

	if (currentToken.type() != TokenType::Identifier) {
		error("Expected identifier.");
	}

	std::string returnType = parseTypeName();
	return std::make_shared<FunctionPrototypeAST>(name, parameters, returnType);
}

std::shared_ptr<FunctionAST> Parser::parseFunctionDef() {
	return std::make_shared<FunctionAST>(parseFunctionPrototype(), parseBlock());
}

std::shared_ptr<MemberFunctionAST> Parser::parseMemberFunctionDef(AccessModifiers accessModifier) {
	return std::make_shared<MemberFunctionAST>(parseFunctionPrototype(), parseBlock(), accessModifier);
}

std::shared_ptr<MemberFunctionAST> Parser::parseConstructorDef(std::string className, AccessModifiers accessModifier) {
	//Eat the class name
	nextToken();

	if (currentTokenAsChar() != '(')  {
		error("Expected ')' in constructor.");
	}

	auto parameters = parseFunctionParameters();
	auto body = parseBlock();

	return std::make_shared<MemberFunctionAST>(
		std::make_shared<FunctionPrototypeAST>(".constructor", parameters, "Void"),
		body,
		accessModifier);
}

std::shared_ptr<ClassDefinitionAST> Parser::parseClassDef() {
	//Eat the 'class'
	nextToken();

	if (currentToken.type() != TokenType::Identifier) {
		error("Expected identifier.");
	}

	auto className = currentToken.strValue;
	nextToken(); //Eat the name

	if (currentTokenAsChar() != '{')  {
		error("Expected '{' after class name.");
	}

	nextToken(); //Eat the '{'

	//Parse the members
	std::vector<std::shared_ptr<FieldDeclarationExpressionAST>> fields;
	std::vector<std::shared_ptr<MemberFunctionAST>> functions;
	auto memberAccessModifier = AccessModifiers::Public;

	while (true) {
		if (currentToken.type() == TokenType::Public) {
			memberAccessModifier = AccessModifiers::Public;
			nextToken();
		} else if (currentToken.type() == TokenType::Private) {
			memberAccessModifier = AccessModifiers::Private;
			nextToken();
		} else if (currentToken.type() == TokenType::Func) {
			functions.push_back(parseMemberFunctionDef(memberAccessModifier));
			nextToken();
			memberAccessModifier = AccessModifiers::Public;
		} else if (currentToken.type() == TokenType::Identifier && currentToken.strValue == className) {
			functions.push_back(parseConstructorDef(className, memberAccessModifier));
			nextToken();
			memberAccessModifier = AccessModifiers::Public;
		} else if (!isSingleCharToken('}')) {
			auto typeName = parseTypeName();

			if (currentToken.type() != TokenType::Identifier) {
				error("Expected identifier after field type.");
			}

			auto fieldName = currentToken.strValue;
			nextToken(); //Eat the field name

			if (!isSingleCharToken(';')) {
				error("Expected ';' after field definition.");
			}

			nextToken(); //Eat the ';'
			fields.push_back(std::make_shared<FieldDeclarationExpressionAST>(typeName, fieldName, memberAccessModifier));
			memberAccessModifier = AccessModifiers::Public;
		}

		if (isSingleCharToken('}')) {
			break;
		}
	}

	return std::make_shared<ClassDefinitionAST>(className, fields, functions);
}

std::shared_ptr<NamespaceDeclarationAST> Parser::parseNamespaceDef() {
	//Eat the 'namespace'
	nextToken();

	//The name
	if (currentToken.type() != TokenType::Identifier) {
		error("Expected identifier.");
	}

	std::string name = currentToken.strValue;
	nextToken(); //Eat the name

	if (currentTokenAsChar() != '{')  {
		error("Expected '{' after namespace name.");
	}

	nextToken(); //Eat the '{'

	//Parse the members
	std::vector<std::shared_ptr<AbstractSyntaxTree>> members;

	while (true) {
		if (currentToken.type() == TokenType::Func) {
			members.push_back(parseFunctionDef());
			nextToken();
		} if (currentToken.type() == TokenType::Class) {
			members.push_back(parseClassDef());
			nextToken();
		} else if (currentToken.type() == TokenType::Namespace) {
			members.push_back(parseNamespaceDef());
			nextToken();
		} else if (currentToken.type() == TokenType::Using) {
			members.push_back(parseUsingNamespaceExpression());
			nextToken();
		} else if (!isSingleCharToken('}')) {
			error("Expected function, class,namespace definition or using within namespace definition.");
		}

		if (isSingleCharToken('}')) {
			break;
		}
	}

	return std::make_shared<NamespaceDeclarationAST>(name, members);
}

std::shared_ptr<ProgramAST> Parser::parse() {
	nextToken();
	std::vector<std::shared_ptr<AbstractSyntaxTree>> globalMembers;
	std::vector<std::shared_ptr<NamespaceDeclarationAST>> namespaces;
	
	while (true) {
		switch (currentToken.type()) {
			case TokenType::EndOfFile:
				{
					auto globalNamespace = std::make_shared<NamespaceDeclarationAST>("global", globalMembers);
					namespaces.push_back(globalNamespace);
					return std::make_shared<ProgramAST>(namespaces);
				}
			case TokenType::Namespace:
				namespaces.push_back(parseNamespaceDef());
				nextToken();
				break;
			case TokenType::Func:
				globalMembers.push_back(parseFunctionDef());
				nextToken();
				break;
			case TokenType::Class:
				globalMembers.push_back(parseClassDef());
				nextToken();
				break;
			case TokenType::Using:
				globalMembers.push_back(parseUsingNamespaceExpression());
				nextToken();
				break;
			default:
				error("Invalid token: " + currentToken.asString());
				break;
		}
	}
}