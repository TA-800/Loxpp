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

    if (argc != 2)
    {
        std::cerr << "Usage: GenerateAST <output directory>"
                  << "\n";
        return 64;
    }

    /* const char *outputDir = argv[1]; */
    std::string outputDir = argv[1];
    const char *baseName = "Expr";
    const std::vector<std::string> types = {"Binary   : Expr left, Token op, Expr right", "Grouping : Expr expression",
                                            "Literal  : void *value", "Unary    : Token op, Expr right"};
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

    // Include guard
    headerFile << "#ifndef " << baseName << "_HPP"
               << "\n";
    headerFile << "#define " << baseName << "_HPP"
               << "\n";

    // Include Token.hpp
    headerFile << "#include \"Token.hpp\""
               << "\n";

    // Start of abstract class definition
    headerFile << "class " << baseName << "{\n";

    // Define the public section of the class
    // destructor = default
    headerFile << "public:\n";
    headerFile << "virtual ~" << baseName << "() = default;\n";

    // End of abstract class definition
    headerFile << "};\n";

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
    // Write class definition
    // E.g. Binary : Expr left, Token op, Expr right
    // -> class Binary : public Expr {
    //      Expr left;
    //      Token op;
    //      Expr right;
    //
    //      Binary(Expr &left, Token &op, Expr &right) : left(left), op(op), right(right) {}
    // }

    // Extract the fields
    // Vector of <type, name>, e.g. {Expr, left}, {Token, op}, {Expr, right}
    std::vector<std::pair<std::string, std::string>> fields;
    std::istringstream iss(fieldList);
    std::string field;

    while (std::getline(iss, field, ',')) // Takes in params by reference
    {
        std::string type = field.substr(0, field.find(" "));
        std::string name = field.substr(field.find(" ") + 1);
        fields.push_back(std::make_pair(type, name));
    }

    std::string constructorParams;
    std::string initializationParams;

    // Start class
    headerFile << "class " << className << " : public " << baseName << "{\n";

    // Define the fields
    // E.g. Expr left; Token op; Expr right;
    for (const auto &field : fields)
    {
        headerFile << field.first << " " << field.second << ";\n";

        constructorParams += field.first + " &" + field.second;          // Expr &left
        initializationParams += field.second + "(" + field.second + ")"; // left(left)
        if (field != fields.back())
        {
            constructorParams += ", ";
            initializationParams += ", ";
        }
    }
    headerFile << "\n";

    // Constructor (take by reference)
    // E.g. Binary(Expr &left, Token &op, Expr &right) : left(left), op(op), right(right) {}

    // Binary(Expr &left, Token &op, Expr &right)
    headerFile << className << "(" << constructorParams;
    // ) : left(left), op(op), right(right) {}
    headerFile << ") : " << initializationParams << " {}\n";

    // End class
    headerFile << "};\n";
}
