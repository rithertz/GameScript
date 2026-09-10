#pragma once

//==============================================================================
// Statements.hpp
//
// Defines the statement nodes, statement types, and program structure used
// to represent GameScript statements in the Abstract Syntax Tree.
//==============================================================================

#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript {

// Represents the supported movement directions in GameScript.
enum class MoveDir {
    Forward,
    Backward,
    Left,
    Right
};

// Converts a movement direction to its string representation.
std::string moveDirToString(MoveDir dir);

// Represents the supported turning directions in GameScript.
enum class TurnDir {
    Left,
    Right
};

// Converts a turning direction to its string representation.
std::string turnDirToString(TurnDir dir);

// Represents the different game actions supported by GameScript.
enum class ActionKind {
    Attack,
    Defend,
    Jump,
    Interact,
    Retreat
};

// Converts an action kind to its string representation.
std::string actionKindToString(ActionKind kind);

// Stream operators for displaying statement-related enum values.
inline std::ostream& operator<<(std::ostream& os, MoveDir dir) {
    return os << moveDirToString(dir);
}

inline std::ostream& operator<<(std::ostream& os, TurnDir dir) {
    return os << turnDirToString(dir);
}

inline std::ostream& operator<<(std::ostream& os, ActionKind kind) {
    return os << actionKindToString(kind);
}

// Base class for all statement nodes in the AST.
class Stmt : public ASTNode {
public:
    explicit Stmt(ASTNodeType type, SourceSpan span = SourceSpan())
        : ASTNode(type, span) {}
};

// Owning pointer to a statement node.
using StmtPtr = std::unique_ptr<Stmt>;

// Represents a variable declaration statement with an optional initializer.
class VarDeclStmt : public Stmt {
public:
    VarDeclStmt(std::string varName, ExprPtr initializer, SourceSpan span)
        : Stmt(ASTNodeType::VarDeclStmt, span),
          varName_(std::move(varName)),
          initializer_(std::move(initializer)) {}

    const std::string& getVarName() const { return varName_; }
    Expr* getInitializer() const { return initializer_.get(); }
    ExprPtr& getInitializerPtr() { return initializer_; }

    // Visitor dispatch for variable declaration statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    std::string varName_;
    ExprPtr initializer_;
};

// Represents a movement statement with a direction and distance.
class MoveStmt : public Stmt {
public:
    MoveStmt(MoveDir direction, ExprPtr distance, SourceSpan span)
        : Stmt(ASTNodeType::MoveStmt, span),
          direction_(direction),
          distance_(std::move(distance)) {}

    MoveDir getDirection() const { return direction_; }
    Expr* getDistance() const { return distance_.get(); }
    ExprPtr& getDistancePtr() { return distance_; }

    // Visitor dispatch for movement statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    MoveDir direction_;
    ExprPtr distance_;
};

// Represents a turn statement with a direction and number of degrees.
class TurnStmt : public Stmt {
public:
    TurnStmt(TurnDir direction, ExprPtr degrees, SourceSpan span)
        : Stmt(ASTNodeType::TurnStmt, span),
          direction_(direction),
          degrees_(std::move(degrees)) {}

    TurnDir getDirection() const { return direction_; }
    Expr* getDegrees() const { return degrees_.get(); }
    ExprPtr& getDegreesPtr() { return degrees_; }

    // Visitor dispatch for turn statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    TurnDir direction_;
    ExprPtr degrees_;
};

// Represents a game action statement such as attack, defend, or jump.
class ActionStmt : public Stmt {
public:
    ActionStmt(ActionKind action, SourceSpan span)
        : Stmt(ASTNodeType::ActionStmt, span), action_(action) {}

    ActionKind getAction() const { return action_; }

    // Visitor dispatch for action statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ActionKind action_;
};

// Represents a conditional statement with then and optional else branches.
class IfStmt : public Stmt {
public:
    IfStmt(ExprPtr condition, std::vector<StmtPtr> thenBranch,
           std::vector<StmtPtr> elseBranch, SourceSpan span)
        : Stmt(ASTNodeType::IfStmt, span),
          condition_(std::move(condition)),
          thenBranch_(std::move(thenBranch)),
          elseBranch_(std::move(elseBranch)) {}

    Expr* getCondition() const { return condition_.get(); }
    ExprPtr& getConditionPtr() { return condition_; }
    const std::vector<StmtPtr>& getThenBranch() const { return thenBranch_; }
    std::vector<StmtPtr>& getThenBranch() { return thenBranch_; }
    const std::vector<StmtPtr>& getElseBranch() const { return elseBranch_; }
    std::vector<StmtPtr>& getElseBranch() { return elseBranch_; }

    // Visitor dispatch for conditional statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> thenBranch_;
    std::vector<StmtPtr> elseBranch_;
};

// Represents a repeat loop with a count expression and body.
class RepeatStmt : public Stmt {
public:
    RepeatStmt(ExprPtr countExpr, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::RepeatStmt, span),
          countExpr_(std::move(countExpr)),
          body_(std::move(body)) {}

    Expr* getCountExpr() const { return countExpr_.get(); }
    ExprPtr& getCountExprPtr() { return countExpr_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    // Visitor dispatch for repeat statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ExprPtr countExpr_;
    std::vector<StmtPtr> body_;
};

// Represents a while loop with a condition and body.
class WhileStmt : public Stmt {
public:
    WhileStmt(ExprPtr condition, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::WhileStmt, span),
          condition_(std::move(condition)),
          body_(std::move(body)) {}

    Expr* getCondition() const { return condition_.get(); }
    ExprPtr& getConditionPtr() { return condition_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    // Visitor dispatch for while statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> body_;
};

// Represents a function declaration with parameters and a statement body.
class FunctionDeclStmt : public Stmt {
public:
    FunctionDeclStmt(std::string name, std::vector<std::string> params,
                     std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::FunctionDeclStmt, span),
          name_(std::move(name)),
          params_(std::move(params)),
          body_(std::move(body)) {}

    const std::string& getName() const { return name_; }
    const std::vector<std::string>& getParams() const { return params_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    // Visitor dispatch for function declarations.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    std::string name_;
    std::vector<std::string> params_;
    std::vector<StmtPtr> body_;
};

// Represents a function call with a function name and arguments.
class FunctionCallStmt : public Stmt {
public:
    FunctionCallStmt(std::string name, std::vector<ExprPtr> args, SourceSpan span)
        : Stmt(ASTNodeType::FunctionCallStmt, span),
          name_(std::move(name)),
          args_(std::move(args)) {}

    const std::string& getName() const { return name_; }
    const std::vector<ExprPtr>& getArgs() const { return args_; }
    std::vector<ExprPtr>& getArgs() { return args_; }

    // Visitor dispatch for function calls.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    std::string name_;
    std::vector<ExprPtr> args_;
};

// Represents a conditional event handler with a condition and body.
class WhenStmt : public Stmt {
public:
    WhenStmt(ExprPtr condition, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::WhenStmt, span),
          condition_(std::move(condition)),
          body_(std::move(body)) {}

    Expr* getCondition() const { return condition_.get(); }
    ExprPtr& getConditionPtr() { return condition_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    // Visitor dispatch for when statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> body_;
};

// Represents a statement consisting of a single expression.
class ExprStmt : public Stmt {
public:
    explicit ExprStmt(ExprPtr expr, SourceSpan span)
        : Stmt(ASTNodeType::ExprStmt, span), expr_(std::move(expr)) {}

    Expr* getExpr() const { return expr_.get(); }
    ExprPtr& getExprPtr() { return expr_; }

    // Visitor dispatch for expression statements.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the statement.
    std::string toString() const override;

private:
    ExprPtr expr_;
};

// Represents the root node of a GameScript program.
class Program : public ASTNode {
public:
    explicit Program(std::vector<StmtPtr> statements, SourceSpan span = SourceSpan())
        : ASTNode(ASTNodeType::Program, span),
          statements_(std::move(statements)) {}

    const std::vector<StmtPtr>& getStatements() const { return statements_; }
    std::vector<StmtPtr>& getStatements() { return statements_; }

    // Adds a statement to the program.
    void addStatement(StmtPtr stmt) { statements_.push_back(std::move(stmt)); }

    // Visitor dispatch for the program node.
    void accept(ASTVisitor& visitor) override;

    // Returns a human-readable representation of the program.
    std::string toString() const override;

private:
    std::vector<StmtPtr> statements_;
};

} // namespace gamescript