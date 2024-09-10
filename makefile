# PATHS $
build_dir = build
src_dir = src

# Build arguments #
INCLUDES = -I $(src_dir)
GPPARGS = -std=c++17 -Wall

CODEGENFILES = code_generation/generator.cpp arg_parser/arg_parser.cpp parallel_regex/pregex.cpp

# Intermediaries #
CODEGEN = $(addprefix $(build_dir)/, $(CODEGENFILES:.cpp=.o))

# Build Objects #
$(build_dir)/%.o : $(src_dir)/%.cpp
	mkdir -p $(@D)
	g++ $(GPPARGS) $(INCLUDES) -c $< -o $@

code_generator: $(CODEGEN)
	g++ $(GPPARGS) $(CODEGEN) -o $(build_dir)/code_generator 

clean: 
	 rm -rf $(build_dir)/* 
