#pragma once

#include "gamescript/ast/ASTNode.hpp"
#include "gamescript/ast/Expressions.hpp"
#include <string>
#include <vector>
#include <memory>

namespace gamescript {

enum class MoveDir {
    Forward,
    Backward,
    Left,
    Right
};

std::string moveDirToString(MoveDir dir);

enum class TurnDir {
    Left,
    Right
};

std::string turnDirToString(TurnDir dir);

enum class ActionKind {
    Attack,
    Defend,
    Jump,
    Interact,
    Retreat
};

std::string actionKindToString(ActionKind kind);

inline std::ostream& operator<<(std::ostream& os, MoveDir dir) { return os << moveDirToString(dir); }
inline std::ostream& operator<<(std::ostream& os, TurnDir dir) { return os << turnDirToString(dir); }
inline std::ostream& operator<<(std::ostream& os, ActionKind kind) { return os << actionKindToString(kind); }

class Stmt : public ASTNode {
public:
    explicit Stmt(ASTNodeType type, SourceSpan span = SourceSpan())
        : ASTNode(type, span) {}
};

using StmtPtr = std::unique_ptr<Stmt>;

class VarDeclStmt : public Stmt {
public:
    VarDeclStmt(std::string varName, ExprPtr initializer, SourceSpan span)
        : Stmt(ASTNodeType::VarDeclStmt, span), varName_(std::move(varName)), initializer_(std::move(initializer)) {}

    const std::string& getVarName() const { return varName_; }
    Expr* getInitializer() const { return initializer_.get(); }
    ExprPtr& getInitializerPtr() { return initializer_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string varName_;
    ExprPtr initializer_;
};

class MoveStmt : public Stmt {
public:
    MoveStmt(MoveDir direction, ExprPtr distance, SourceSpan span)
        : Stmt(ASTNodeType::MoveStmt, span), direction_(direction), distance_(std::move(distance)) {}

    MoveDir getDirection() const { return direction_; }
    Expr* getDistance() const { return distance_.get(); }
    ExprPtr& getDistancePtr() { return distance_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    MoveDir direction_;
    ExprPtr distance_;
};

class TurnStmt : public Stmt {
public:
    TurnStmt(TurnDir direction, ExprPtr degrees, SourceSpan span)
        : Stmt(ASTNodeType::TurnStmt, span), direction_(direction), degrees_(std::move(degrees)) {}

    TurnDir getDirection() const { return direction_; }
    Expr* getDegrees() const { return degrees_.get(); }
    ExprPtr& getDegreesPtr() { return degrees_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    TurnDir direction_;
    ExprPtr degrees_;
};

class ActionStmt : public Stmt {
public:
    ActionStmt(ActionKind action, SourceSpan span)
        : Stmt(ASTNodeType::ActionStmt, span), action_(action) {}

    ActionKind getAction() const { return action_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ActionKind action_;
};

class IfStmt : public Stmt {
public:
    IfStmt(ExprPtr condition, std::vector<StmtPtr> thenBranch, std::vector<StmtPtr> elseBranch, SourceSpan span)
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

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> thenBranch_;
    std::vector<StmtPtr> elseBranch_;
};

class RepeatStmt : public Stmt {
public:
    RepeatStmt(ExprPtr countExpr, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::RepeatStmt, span), countExpr_(std::move(countExpr)), body_(std::move(body)) {}

    Expr* getCountExpr() const { return countExpr_.get(); }
    ExprPtr& getCountExprPtr() { return countExpr_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExprPtr countExpr_;
    std::vector<StmtPtr> body_;
};

class WhileStmt : public Stmt {
public:
    WhileStmt(ExprPtr condition, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::WhileStmt, span), condition_(std::move(condition)), body_(std::move(body)) {}

    Expr* getCondition() const { return condition_.get(); }
    ExprPtr& getConditionPtr() { return condition_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> body_;
};

class FunctionDeclStmt : public Stmt {
public:
    FunctionDeclStmt(std::string name, std::vector<std::string> params, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::FunctionDeclStmt, span),
          name_(std::move(name)),
          params_(std::move(params)),
          body_(std::move(body)) {}

    const std::string& getName() const { return name_; }
    const std::vector<std::string>& getParams() const { return params_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string name_;
    std::vector<std::string> params_;
    std::vector<StmtPtr> body_;
};

class FunctionCallStmt : public Stmt {
public:
    FunctionCallStmt(std::string name, std::vector<ExprPtr> args, SourceSpan span)
        : Stmt(ASTNodeType::FunctionCallStmt, span), name_(std::move(name)), args_(std::move(args)) {}

    const std::string& getName() const { return name_; }
    const std::vector<ExprPtr>& getArgs() const { return args_; }
    std::vector<ExprPtr>& getArgs() { return args_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::string name_;
    std::vector<ExprPtr> args_;
};

class WhenStmt : public Stmt {
public:
    WhenStmt(ExprPtr condition, std::vector<StmtPtr> body, SourceSpan span)
        : Stmt(ASTNodeType::WhenStmt, span), condition_(std::move(condition)), body_(std::move(body)) {}

    Expr* getCondition() const { return condition_.get(); }
    ExprPtr& getConditionPtr() { return condition_; }
    const std::vector<StmtPtr>& getBody() const { return body_; }
    std::vector<StmtPtr>& getBody() { return body_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExprPtr condition_;
    std::vector<StmtPtr> body_;
};

class ExprStmt : public Stmt {
public:
    explicit ExprStmt(ExprPtr expr, SourceSpan span)
        : Stmt(ASTNodeType::ExprStmt, span), expr_(std::move(expr)) {}

    Expr* getExpr() const { return expr_.get(); }
    ExprPtr& getExprPtr() { return expr_; }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    ExprPtr expr_;
};

class Program : public ASTNode {
public:
    explicit Program(std::vector<StmtPtr> statements, SourceSpan span = SourceSpan())
        : ASTNode(ASTNodeType::Program, span), statements_(std::move(statements)) {}

    const std::vector<StmtPtr>& getStatements() const { return statements_; }
    std::vector<StmtPtr>& getStatements() { return statements_; }

    void addStatement(StmtPtr stmt) { statements_.push_back(std::move(stmt)); }

    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;

private:
    std::vector<StmtPtr> statements_;
};

} // namespace gamescript
