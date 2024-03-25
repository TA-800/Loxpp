# Compiler
CC := g++

# Directories
SRCDIR := source
BUILDDIR := build
OBJDIR := $(BUILDDIR)/objects
BINDIR := $(BUILDDIR)/bin

# Files
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
EXEC := $(BINDIR)/run

# Compiler flags
CFLAGS := -I$(SRCDIR)/headers -Wall -Wextra -pedantic -std=c++14

# Default target
all: $(EXEC)

# Rule to build the executable
$(EXEC): $(OBJS)
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Rule to compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(BUILDDIR)

# Dependency generation
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

# Generate dependencies
$(OBJDIR)/%.d: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	@$(CC) $(CFLAGS) -MM -MT '$(OBJDIR)/$*.o' $< > $@
