build_dir = build
build_codegen = $(build_dir)/code_generation
codegen_dir = src/code_generation

$(build_codegen)/generator.o: $(codegen_dir)/generator.cpp | $(build_codegen)
	g++ -std=c++17 -c $(codegen_dir)/generator.cpp -I $(codegen_dir) -o $(build_codegen)/generator.o

$(build_codegen)/arg_parser.o: $(codegen_dir)/arg_parser.cpp | $(build_codegen)
	g++ -std=c++17 -c $(codegen_dir)/arg_parser.cpp -I $(codegen_dir) -o $(build_codegen)/arg_parser.o

code_generator: $(build_codegen)/generator.o $(build_codegen)/arg_parser.o
	g++ -std=c++17 $(build_codegen)/generator.o $(build_codegen)/arg_parser.o -o $(build_dir)/code_generator 

$(build_codegen):
	mkdir $@

clean: 
	 rm -rf $(build_dir)/* 
