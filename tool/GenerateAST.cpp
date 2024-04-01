#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

void defineAst(std::string &outputDir, const char *baseName, const std::vector<std::string> &types);
void defineType(std::ofstream &headerFile, const char *baseName, const std::string &className,
                const std::string &fieldList);

/*
 * It is tedious to write all the Expr subclasses that represent the AST nodes.
 * This script will generate the classes for us.
 */

int main(int argc, char *argv[])
{

    std::string outputDir = "source/headers";
    /* const char *baseName = "Expr"; */
    /* const std::vector<std::string> types = { */
    /*     "Call     : std::unique_ptr<Expr> callee, Token paren, std::vector<std::unique_ptr<Expr>> arguments", */
    /*     "Assign   : Token name, std::unique_ptr<Expr> value", */
    /*     "Binary   : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right", */
    /*     "Grouping : std::unique_ptr<Expr> expression", */
    /*     "Literal  : TokenInfo::Type type, void *value", */
    /*     "Logical  : std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right", */
    /*     "Unary    : Token op, std::unique_ptr<Expr> right", */
    /*     "Variable : Token name", */
    /* }; */
    const char *baseName = "Stmt";
    const std::vector<std::string> types = {
        "If : std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch, std::unique_ptr<Stmt> elseBranch",

        "While : std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body",
        "Block      : std::vector<std::unique_ptr<Stmt>> statements",
        "Break",
        "Expression : std::unique_ptr<Expr> expression",
        "Print      : std::unique_ptr<Expr> expression",
        "Var        : Token name, std::unique_ptr<Expr> initializer",
        "Function   : Token name, std::vector<Token> params, std::vector<std::unique_ptr<Stmt>> body",
    };

    defineAst(outputDir, baseName, types);
};

void defineAst(std::string &outputDir, const char *baseName, const std::vector<std::string> &types)
{
    // Header files that contain information (bag of data) about the AST nodes

    // File writer
    std::ofstream headerFile(outputDir + "/" + baseName + ".hpp");
    if (!headerFile)
    {
        std::cerr << "Could not open file for writing"
                  << "\n";
        return;
    }

    // Subclasses
    // E.g. Binary, Grouping, Literal, Unary
    std::vector<std::string> subclasses;
    for (const std::string &type : types)
    {
        std::string className = type.substr(0, type.find(":") - 1);
        // Remove spaces after the class name, e.g. "Unary   " -> "Unary"
        className = className.substr(className.find_first_not_of(" "), className.find_last_not_of(" ") + 1);
        subclasses.push_back(className);
    }

    // Includes
    headerFile << "#ifndef " << baseName << "_HPP"
               << "\n";
    headerFile << "#define " << baseName << "_HPP"
               << "\n\n";
    headerFile << "#include \"Token.hpp\""
               << "\n";
    headerFile << "#include \"Expr.hpp\""
               << "\n";
    headerFile << "#include <memory>"
               << "\n\n";

    // Forward declarations
    // e.g. class Binary; class Grouping; class Literal; class Unary;
    for (const std::string &subclassName : subclasses)
    {
        headerFile << "class " << subclassName << ";"
                   << "\n";
    }

    // ExprVisitor or StmtVisitor
    headerFile << "class " << baseName << "Visitor{"
               << "\n";
    headerFile << "public:"
               << "\n";
    for (const std::string &subclassName : subclasses)
    {
        // e.g. virtual void visitBinaryExpr(const Binary &expr) = 0;
        // e.g. virtual void visitPrintStmt(const Print &stmt) = 0;
        headerFile << "virtual void visit" << subclassName << baseName << "(const " << subclassName << " &" << baseName
                   << ") = 0;"
                   << "\n";
    }
    headerFile << " };"
               << "\n";

    // Start of abstract class definition
    headerFile << "class " << baseName << "{"
               << "\n";
    // Define the public section of the class
    headerFile << "public:"
               << "\n";
    headerFile << "virtual ~" << baseName << "() = default;"
               << "\n";
    headerFile << "virtual void accept(" << baseName << "Visitor &visitor) = 0;"
               << "\n";
    // End of abstract class definition
    headerFile << "};"
               << "\n";

    // Define the subclasses
    // For each type in the vector
    // Types = subclasses, fields = params of subclasses constructors
    // E.g. Binary : Expr left, Token op, Expr right
    for (const std::string &type : types)
    {
        std::string className = type.substr(0, type.find(":") - 1);
        std::string fields = type.substr(type.find(":") + 1);
        defineType(headerFile, baseName, className, fields);
    }

    // End of guard (end of file)
    headerFile << "#endif"
               << "\n";

    // Close the file
    headerFile.close();
}

void defineType(std::ofstream &headerFile, const char *baseName, const std::string &className,
                const std::string &fieldList)
{
    // Extract the fields
    // Vector of <type, name>, e.g. {Expr, left}, {Token, op}, {Expr, right}
    std::vector<std::pair<std::string, std::string>> fields;
    std::istringstream iss(fieldList);
    std::string field;

    while (std::getline(iss, field, ',')) // Takes in params by reference
    {

        // Remove leading/trailing whitespaces
        field = field.substr(field.find_first_not_of(" "), field.find_last_not_of(" ") + 1);
        std::string type = field.substr(0, field.find(" "));
        std::string name = field.substr(field.find(" ") + 1);
        fields.push_back(std::make_pair(type, name));
    }

    std::string constructorParams;
    std::string initializationParams;

    // Start class
    headerFile << "class " << className << " : public " << baseName << " {"
               << "\n";
    headerFile << "public:"
               << "\n";

    // Define the fields
    // E.g. Expr left; Token op; Expr right;
    for (const auto &field : fields)
    {
        // NOTE: Structured bindings
        auto &[type, name] = field;
        headerFile << type << " " << name << ";"
                   << "\n";

        // If type contains vector of unique_ptr then manually construct the vector
        if (type.find("unique_ptr") != std::string::npos)
        {
            // E.g. Binary(std::unique_ptr<Expr> &left, Token op, std::unique_ptr<Expr> &right);
            constructorParams += type + " &" + name;
            initializationParams += name + "(std::move(" + name + "))";
        }
        else
        {
            constructorParams += type + " " + name;
            // If raw ptr then don't do *ptr( *ptr ), just ptr(ptr)
            // Example: void *literal; -> literal(literal)
            if (type.find("*") != std::string::npos)
            {
                std::string temp = name.substr(1);
                initializationParams += temp + "(" + temp + ")";
            }
            else
                initializationParams += name + "(" + name + ")";
        }

        // Add commas if not the last field
        if (field != fields.back())
        {
            constructorParams += ", ";
            initializationParams += ", ";
        }
    }
    headerFile << "\n";

    // E.g. Binary(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right) :
    // left(std::move(left)), op(op), right(std::move(right)) {}
    headerFile << className << "(" << constructorParams << ") : " << initializationParams << " {}"
               << "\n";

    // void accept(Visitor &visitor) override { visitor.visit[className][baseName](*this); }
    headerFile << "void accept( " << baseName << "Visitor &visitor) override { visitor.visit" << className << baseName
               << "(*this); }"
               << "\n";

    // End class
    headerFile << "};"
               << "\n";
}
