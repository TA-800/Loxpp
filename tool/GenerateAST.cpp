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

    /* if (argc != 2) */
    /* { */
    /*     std::cerr << "Usage: GenerateAST <output directory>" << std::endl; */
    /*     return 64; */
    /* } */

    /* const char *outputDir = argv[1]; */
    /* std::string outputDir = argv[1]; */
    std::string outputDir = "source/headers";
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
        std::cerr << "Could not open file for writing" << std::endl;
        return;
    }

    // Include guard
    headerFile << "#ifndef " << baseName << "_HPP" << std::endl;
    headerFile << "#define " << baseName << "_HPP" << std::endl;

    // Include Token.hpp
    headerFile << "#include \"Token.hpp\"" << std::endl;

    // Start of abstract class definition
    headerFile << "template <typename T>" << std::endl;
    headerFile << "class " << baseName << "{" << std::endl;

    // Define the public section of the class
    // destructor = default
    headerFile << "public:" << std::endl;
    headerFile << "virtual ~" << baseName << "() = default;" << std::endl;
    headerFile << "virtual T accept(Visitor<T> visitor) = 0;" << std::endl;

    // End of abstract class definition
    headerFile << "};" << std::endl;

    // Define the subclasses
    // For each type in the vector
    // Types = subclasses, fields = params of subclasses constructors
    // E.g. Binary : Expr left, Token op, Expr right

    for (const std::string type : types)
    {

        std::string className = type.substr(0, type.find(":") - 1);
        std::string fields = type.substr(type.find(":") + 1);

        defineType(headerFile, baseName, className, fields);
    }

    // End of guard (end of file)
    headerFile << "#endif" << std::endl;

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

        // Remove leading/trailing whitespaces
        field = field.substr(field.find_first_not_of(" "), field.find_last_not_of(" ") + 1);

        std::string type = field.substr(0, field.find(" "));
        std::string name = field.substr(field.find(" ") + 1);
        fields.push_back(std::make_pair(type, name));
    }

    std::string constructorParams;
    std::string initializationParams;

    // Start class
    headerFile << "class " << className << " : public " << baseName << "{" << std::endl;

    // Define the fields
    // E.g. Expr left; Token op; Expr right;
    for (const auto &field : fields)
    {
        headerFile << field.first << " " << field.second << ";" << std::endl;

        constructorParams += field.first + " &" + field.second;          // Expr &left
        initializationParams += field.second + "(" + field.second + ")"; // left(left)
        if (field != fields.back())
        {
            constructorParams += ", ";
            initializationParams += ", ";
        }
    }
    headerFile << std::endl;

    // Constructor (take by reference)
    // E.g. Binary(Expr &left, Token &op, Expr &right) : left(left), op(op), right(right) {}

    // Binary(Expr &left, Token &op, Expr &right)
    headerFile << className << "(" << constructorParams;
    // ) : left(left), op(op), right(right) {}
    headerFile << ") : " << initializationParams << " {}" << std::endl;

    // End class
    headerFile << "};" << std::endl;
}
