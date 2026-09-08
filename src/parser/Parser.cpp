#include "gamescript/parser/Parser.hpp"
#include <stdexcept>

namespace gamescript {

Parser::Parser(std::vector<Token> tokens, DiagnosticEngine* diagnostics)
    : tokens_(std::move(tokens)), diagnostics_(diagnostics) {}

const Token& Parser::peek() const {
    if (current_ >= tokens_.size()) {
        return tokens_.back(); // EndOfFile token
    }
    return tokens_[current_];
}

const Token& Parser::previous() const {
    if (current_ == 0) return tokens_[0];
    return tokens_[current_ - 1];
}

bool Parser::isAtEnd() const {
    return peek().is(TokenType::EndOfFile);
}

Token Parser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().is(type);
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchAny(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    
    SourceSpan span = peek().span;
    std::string msg = errorMessage + " Found '" + peek().lexeme + "'.";
    if (diagnostics_) {
        diagnostics_->reportSyntaxError(span, msg);
    }
    hasInternalErrors_ = true;
    throw std::runtime_error(msg);
}

void Parser::skipNewlines() {
    while (check(TokenType::Newline)) {
        advance();
    }
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().is(TokenType::Newline) || previous().is(TokenType::Dedent)) return;
        switch (peek().type) {
            case TokenType::Set:
            case TokenType::Player:
            case TokenType::If:
            case TokenType::Repeat:
            case TokenType::While:
            case TokenType::Function:
            case TokenType::Call:
            case TokenType::When:
                return;
            default:
                advance();
                break;
        }
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    std::vector<StmtPtr> statements;
    SourceLocation startLoc = peek().span.start;

    skipNewlines();
    while (!isAtEnd()) {
        try {
            StmtPtr stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const std::exception&) {
            synchronize();
        }
        skipNewlines();
    }

    SourceLocation endLoc = previous().span.end;
    return std::make_unique<Program>(std::move(statements), SourceSpan(startLoc, endLoc));
}

StmtPtr Parser::parseStatement() {
    skipNewlines();
    if (isAtEnd()) return nullptr;

    switch (peek().type) {
        case TokenType::Set:
            return parseVarDecl();
        case TokenType::Player:
            return parsePlayerStatement();
        case TokenType::If:
            return parseIfStatement();
        case TokenType::Repeat:
            return parseRepeatStatement();
        case TokenType::While:
            return parseWhileStatement();
        case TokenType::Function:
            return parseFunctionDecl();
        case TokenType::Call:
            return parseFunctionCall();
        case TokenType::When:
            return parseWhenStatement();
        default:
            return parseExprStatement();
    }
}

StmtPtr Parser::parseVarDecl() {
    Token setTok = consume(TokenType::Set, "Expected 'set' keyword.");
    Token nameTok = peek();
    if (nameTok.type != TokenType::Identifier && !nameTok.isKeyword()) {
        std::string msg = "Expected variable name after 'set'. Found '" + nameTok.lexeme + "'.";
        if (diagnostics_) diagnostics_->reportSyntaxError(nameTok.span, msg);
        hasInternalErrors_ = true;
        throw std::runtime_error(msg);
    }
    advance();
    consume(TokenType::Equal, "Expected '=' after variable name.");
    ExprPtr initExpr = parseExpression();

    SourceSpan span(setTok.span.start, initExpr ? initExpr->getSpan().end : nameTok.span.end);
    return std::make_unique<VarDeclStmt>(nameTok.lexeme, std::move(initExpr), span);
}

StmtPtr Parser::parsePlayerStatement() {
    Token playerTok = consume(TokenType::Player, "Expected 'player'.");

    if (match(TokenType::Move)) {
        Token dirTok = peek();
        MoveDir dir = MoveDir::Forward;
        if (match(TokenType::Forward)) dir = MoveDir::Forward;
        else if (match(TokenType::Backward)) dir = MoveDir::Backward;
        else if (match(TokenType::Left)) dir = MoveDir::Left;
        else if (match(TokenType::Right)) dir = MoveDir::Right;
        else {
            std::string msg = "Expected movement direction ('forward', 'backward', 'left', 'right') after 'player move'. Found '" + dirTok.lexeme + "'.";
            if (diagnostics_) diagnostics_->reportSyntaxError(dirTok.span, msg);
            hasInternalErrors_ = true;
            throw std::runtime_error(msg);
        }

        ExprPtr distExpr = parseExpression();
        SourceSpan span(playerTok.span.start, distExpr ? distExpr->getSpan().end : dirTok.span.end);
        return std::make_unique<MoveStmt>(dir, std::move(distExpr), span);
    }

    if (match(TokenType::Turn)) {
        Token dirTok = peek();
        TurnDir dir = TurnDir::Left;
        if (match(TokenType::Left)) dir = TurnDir::Left;
        else if (match(TokenType::Right)) dir = TurnDir::Right;
        else {
            std::string msg = "Expected turn direction ('left', 'right') after 'player turn'. Found '" + dirTok.lexeme + "'.";
            if (diagnostics_) diagnostics_->reportSyntaxError(dirTok.span, msg);
            hasInternalErrors_ = true;
            throw std::runtime_error(msg);
        }

        ExprPtr degExpr = parseExpression();
        SourceSpan span(playerTok.span.start, degExpr ? degExpr->getSpan().end : dirTok.span.end);
        return std::make_unique<TurnStmt>(dir, std::move(degExpr), span);
    }

    // Action keywords
    Token actionTok = peek();
    ActionKind kind = ActionKind::Attack;
    if (match(TokenType::Attack)) kind = ActionKind::Attack;
    else if (match(TokenType::Defend)) kind = ActionKind::Defend;
    else if (match(TokenType::Jump)) kind = ActionKind::Jump;
    else if (match(TokenType::Interact)) kind = ActionKind::Interact;
    else if (match(TokenType::Retreat)) kind = ActionKind::Retreat;
    else {
        std::string msg = "Expected action ('attack', 'defend', 'jump', 'interact', 'retreat', 'move', 'turn') after 'player'. Found '" + actionTok.lexeme + "'.";
        if (diagnostics_) diagnostics_->reportSyntaxError(actionTok.span, msg);
        hasInternalErrors_ = true;
        throw std::runtime_error(msg);
    }

    SourceSpan span(playerTok.span.start, actionTok.span.end);
    return std::make_unique<ActionStmt>(kind, span);
}

std::vector<StmtPtr> Parser::parseBlock() {
    skipNewlines();
    consume(TokenType::Indent, "Expected indented block.");
    std::vector<StmtPtr> body;

    while (!check(TokenType::Dedent) && !isAtEnd()) {
        skipNewlines();
        if (check(TokenType::Dedent) || isAtEnd()) break;
        try {
            StmtPtr stmt = parseStatement();
            if (stmt) body.push_back(std::move(stmt));
        } catch (const std::exception&) {
            synchronize();
        }
        skipNewlines();
    }

    consume(TokenType::Dedent, "Expected dedent at end of block.");
    return body;
}

StmtPtr Parser::parseIfStatement() {
    Token ifTok = consume(TokenType::If, "Expected 'if'.");
    ExprPtr condition = parseCondition();
    consume(TokenType::Colon, "Expected ':' after if condition.");
    
    std::vector<StmtPtr> thenBranch = parseBlock();
    std::vector<StmtPtr> elseBranch;

    skipNewlines();
    if (match(TokenType::Else)) {
        consume(TokenType::Colon, "Expected ':' after 'else'.");
        elseBranch = parseBlock();
    }

    SourceSpan span(ifTok.span.start, previous().span.end);
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch), span);
}

StmtPtr Parser::parseRepeatStatement() {
    Token repTok = consume(TokenType::Repeat, "Expected 'repeat'.");
    ExprPtr countExpr = parseExpression();
    consume(TokenType::Colon, "Expected ':' after repeat count.");
    std::vector<StmtPtr> body = parseBlock();

    SourceSpan span(repTok.span.start, previous().span.end);
    return std::make_unique<RepeatStmt>(std::move(countExpr), std::move(body), span);
}

StmtPtr Parser::parseWhileStatement() {
    Token whileTok = consume(TokenType::While, "Expected 'while'.");
    ExprPtr condition = parseCondition();
    consume(TokenType::Colon, "Expected ':' after while condition.");
    std::vector<StmtPtr> body = parseBlock();

    SourceSpan span(whileTok.span.start, previous().span.end);
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body), span);
}

StmtPtr Parser::parseFunctionDecl() {
    Token funcTok = consume(TokenType::Function, "Expected 'function'.");
    Token nameTok = consume(TokenType::Identifier, "Expected function name.");
    
    std::vector<std::string> params;
    if (match(TokenType::LParen)) {
        if (!check(TokenType::RParen)) {
            do {
                Token param = consume(TokenType::Identifier, "Expected parameter name.");
                params.push_back(param.lexeme);
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RParen, "Expected ')' after parameter list.");
    }

    consume(TokenType::Colon, "Expected ':' after function header.");
    std::vector<StmtPtr> body = parseBlock();

    SourceSpan span(funcTok.span.start, previous().span.end);
    return std::make_unique<FunctionDeclStmt>(nameTok.lexeme, std::move(params), std::move(body), span);
}

StmtPtr Parser::parseFunctionCall() {
    Token callTok = consume(TokenType::Call, "Expected 'call'.");
    Token nameTok = consume(TokenType::Identifier, "Expected function name after 'call'.");

    std::vector<ExprPtr> args;
    if (match(TokenType::LParen)) {
        if (!check(TokenType::RParen)) {
            do {
                args.push_back(parseExpression());
            } while (match(TokenType::Comma));
        }
        consume(TokenType::RParen, "Expected ')' after argument list.");
    }

    SourceSpan span(callTok.span.start, previous().span.end);
    return std::make_unique<FunctionCallStmt>(nameTok.lexeme, std::move(args), span);
}

StmtPtr Parser::parseWhenStatement() {
    Token whenTok = consume(TokenType::When, "Expected 'when'.");
    ExprPtr condition = parseCondition();
    consume(TokenType::Colon, "Expected ':' after when condition.");
    std::vector<StmtPtr> body = parseBlock();

    SourceSpan span(whenTok.span.start, previous().span.end);
    return std::make_unique<WhenStmt>(std::move(condition), std::move(body), span);
}

StmtPtr Parser::parseExprStatement() {
    ExprPtr expr = parseExpression();
    SourceSpan span = expr ? expr->getSpan() : peek().span;
    return std::make_unique<ExprStmt>(std::move(expr), span);
}

// Condition & Expression parsing
ExprPtr Parser::parseCondition() {
    return parseLogicOr();
}

ExprPtr Parser::parseLogicOr() {
    ExprPtr expr = parseLogicAnd();
    while (match(TokenType::Or)) {
        Token opTok = previous();
        ExprPtr right = parseLogicAnd();
        SourceSpan span(expr->getSpan().start, right ? right->getSpan().end : opTok.span.end);
        expr = std::make_unique<BinaryExpr>(BinaryOp::Or, std::move(expr), std::move(right), span);
    }
    return expr;
}

ExprPtr Parser::parseLogicAnd() {
    ExprPtr expr = parseLogicNot();
    while (match(TokenType::And)) {
        Token opTok = previous();
        ExprPtr right = parseLogicNot();
        SourceSpan span(expr->getSpan().start, right ? right->getSpan().end : opTok.span.end);
        expr = std::make_unique<BinaryExpr>(BinaryOp::And, std::move(expr), std::move(right), span);
    }
    return expr;
}

ExprPtr Parser::parseLogicNot() {
    if (match(TokenType::Not)) {
        Token opTok = previous();
        ExprPtr operand = parseLogicNot();
        SourceSpan span(opTok.span.start, operand ? operand->getSpan().end : opTok.span.end);
        return std::make_unique<UnaryExpr>(UnaryOp::Not, std::move(operand), span);
    }
    return parseRelational();
}

ExprPtr Parser::parseRelational() {
    ExprPtr expr = parseExpression();

    BinaryOp op = BinaryOp::Equal;
    bool isRel = true;

    if (match(TokenType::EqualEqual)) op = BinaryOp::Equal;
    else if (match(TokenType::BangEqual)) op = BinaryOp::NotEqual;
    else if (match(TokenType::Less)) op = BinaryOp::Less;
    else if (match(TokenType::LessEqual)) op = BinaryOp::LessEqual;
    else if (match(TokenType::Greater)) op = BinaryOp::Greater;
    else if (match(TokenType::GreaterEqual)) op = BinaryOp::GreaterEqual;
    else isRel = false;

    if (isRel) {
        Token opTok = previous();
        ExprPtr right = parseExpression();
        SourceSpan span(expr ? expr->getSpan().start : opTok.span.start, right ? right->getSpan().end : opTok.span.end);
        return std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), span);
    }

    return expr;
}

ExprPtr Parser::parseExpression() {
    ExprPtr expr = parseTerm();

    while (match(TokenType::Plus) || match(TokenType::Minus)) {
        Token opTok = previous();
        BinaryOp op = (opTok.type == TokenType::Plus) ? BinaryOp::Add : BinaryOp::Subtract;
        ExprPtr right = parseTerm();
        SourceSpan span(expr ? expr->getSpan().start : opTok.span.start, right ? right->getSpan().end : opTok.span.end);
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), span);
    }

    return expr;
}

ExprPtr Parser::parseTerm() {
    ExprPtr expr = parseFactor();

    while (match(TokenType::Star) || match(TokenType::Slash) || match(TokenType::Percent)) {
        Token opTok = previous();
        BinaryOp op = BinaryOp::Multiply;
        if (opTok.type == TokenType::Slash) op = BinaryOp::Divide;
        else if (opTok.type == TokenType::Percent) op = BinaryOp::Modulo;

        ExprPtr right = parseFactor();
        SourceSpan span(expr ? expr->getSpan().start : opTok.span.start, right ? right->getSpan().end : opTok.span.end);
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right), span);
    }

    return expr;
}

ExprPtr Parser::parseFactor() {
    if (match(TokenType::Minus)) {
        Token opTok = previous();
        ExprPtr operand = parseFactor();
        SourceSpan span(opTok.span.start, operand ? operand->getSpan().end : opTok.span.end);
        return std::make_unique<UnaryExpr>(UnaryOp::Negate, std::move(operand), span);
    }
    if (match(TokenType::Plus)) {
        Token opTok = previous();
        ExprPtr operand = parseFactor();
        SourceSpan span(opTok.span.start, operand ? operand->getSpan().end : opTok.span.end);
        return std::make_unique<UnaryExpr>(UnaryOp::Plus, std::move(operand), span);
    }
    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    // Integer literal
    if (match(TokenType::IntegerLiteral)) {
        Token tok = previous();
        return std::make_unique<IntegerLiteralExpr>(tok.intValue, tok.span);
    }

    // String literal
    if (match(TokenType::StringLiteral)) {
        Token tok = previous();
        return std::make_unique<StringLiteralExpr>(tok.lexeme, tok.span);
    }

    // Boolean literals
    if (match(TokenType::True)) {
        return std::make_unique<BoolLiteralExpr>(true, previous().span);
    }
    if (match(TokenType::False)) {
        return std::make_unique<BoolLiteralExpr>(false, previous().span);
    }

    // Sensory condition predicates
    if (match(TokenType::Enemy)) {
        Token startTok = previous();
        consume(TokenType::Nearby, "Expected 'nearby' after 'enemy'.");
        SourceSpan span(startTok.span.start, previous().span.end);
        return std::make_unique<SensoryConditionExpr>(SensoryType::EnemyNearby, span);
    }

    if (match(TokenType::Health)) {
        Token startTok = previous();
        if (match(TokenType::Low)) {
            SourceSpan span(startTok.span.start, previous().span.end);
            return std::make_unique<SensoryConditionExpr>(SensoryType::HealthLow, span);
        }
        return std::make_unique<SensoryConditionExpr>(SensoryType::Health, startTok.span);
    }

    if (match(TokenType::Obstacle)) {
        Token startTok = previous();
        consume(TokenType::Ahead, "Expected 'ahead' after 'obstacle'.");
        SourceSpan span(startTok.span.start, previous().span.end);
        return std::make_unique<SensoryConditionExpr>(SensoryType::ObstacleAhead, span);
    }

    if (match(TokenType::Distance) || (check(TokenType::Identifier) && peek().lexeme == "distance")) {
        Token startTok = advance();
        if (match(TokenType::To) || (check(TokenType::Identifier) && peek().lexeme == "to")) {
            advance();
            consume(TokenType::Enemy, "Expected 'enemy' after 'distance to'.");
            SourceSpan span(startTok.span.start, previous().span.end);
            return std::make_unique<SensoryConditionExpr>(SensoryType::DistanceToEnemy, span);
        }
        return std::make_unique<IdentifierExpr>(startTok.lexeme, startTok.span);
    }

    // Parentheses (Expression)
    if (match(TokenType::LParen)) {
        Token lparen = previous();
        ExprPtr expr = parseCondition();
        Token rparen = consume(TokenType::RParen, "Expected ')' after expression.");
        SourceSpan span(lparen.span.start, rparen.span.end);
        if (expr) expr->setSpan(span);
        return expr;
    }

    // Identifier / Variable
    if (match(TokenType::Identifier)) {
        Token tok = previous();
        return std::make_unique<IdentifierExpr>(tok.lexeme, tok.span);
    }

    Token badTok = peek();
    std::string msg = "Unexpected token '" + badTok.lexeme + "' in expression.";
    if (diagnostics_) {
        diagnostics_->reportSyntaxError(badTok.span, msg);
    }
    hasInternalErrors_ = true;
    advance();
    throw std::runtime_error(msg);
}

} // namespace gamescript
