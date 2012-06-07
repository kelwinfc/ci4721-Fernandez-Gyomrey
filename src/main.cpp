
#include "main.h"
#include "../y.tab.c"

void read_main_args(int argc, char **argv, FILE *in, main_args& args){
    args.to_single_archive = false;
    args.to_separate_archives = false;
    args.ast = false;
    args.tac_with_comments = false;
    args.in = in;
    if (1 == argc) {
        printf("Opciones para %s [-a | -s] [-c] [-t] archivo:\n"
               " -a imprimir en archivo bla.output\n"
               " -s imprimir en archivo bla.* separados\n"
               " -c imprimir el tac con comentarios\n"
               " -t imprimir el ast\n", argv[0]);
        exit(1);
    }
    for (int i = 1; i < argc; ++i) {
        if (i + 1 == argc) {
            args.in = fopen(argv[i], "r");
            if (!args.in) {
                printf("Archivo '%s' no existe\n", argv[i]);
                exit(1);
            }
        } else if (!strcmp("-a", argv[i])) {
            args.to_separate_archives = false;
            args.to_single_archive = true;
        } else if (!strcmp("-s", argv[i])) {
            args.to_separate_archives = true;
            args.to_single_archive = false;
        } else if (!strcmp("-c", argv[i])) {
            args.tac_with_comments = true;
        } else if (!strcmp("-t", argv[i])) {
            args.ast = true;
        } else {
            printf("Opciones para %s [-a | -s] [-c] [-t] archivo:\n"
                   " -a imprimir en archivo bla.output\n"
                   " -s imprimir en archivo bla.* separados\n"
                   " -c imprimir el tac con comentarios\n"
                   " -t imprimir el ast\n", argv[0]);
            exit(1);
        }
    }
}

void redirect_stdout(const char *f = 0) {
    static FILE *fd = 0, *stdout_back = 0;
    static streambuf *rdbuf_back = 0;
    static ofstream filestr;
    if (0 == stdout_back) {
        stdout_back = stdout;
        rdbuf_back = cout.rdbuf();
    }
    if (0 != fd && stdout != fd) {
        fclose(fd);
        filestr.close();
    }
    if (0 == f) {
        stdout = stdout_back;
        cout.rdbuf(rdbuf_back);
    } else {
        fd = stdout = fopen(f, "w");
        filestr.open(f);
        cout.rdbuf(filestr.rdbuf());
    }
}

int main (int argc,char **argv)
{
    FILE * stderr_back = stderr;
    logger = new llog();
    p = new AST_program();
    block *b = new block(true);
    main_args args;
    read_main_args(argc, argv, stdin, args);
    yyin = args.in;
    do {
        yyparse();
    } while (!feof(yyin));

    if (args.to_single_archive){
        redirect_stdout("bla.output");
    }
    
    symbol_table* st = new symbol_table();
    
    p->fill_and_check(st);
    
    delete st;
    
    if ( logger->exists_registered_error() ){
        logger->dump(cerr);
        cerr << "-------------------------------------------------------" << endl << endl;
    }
    
    if (args.to_separate_archives) {
        redirect_stdout("bla.types");
    }
    types.dump();
    if (args.to_separate_archives) {
        redirect_stdout("bla.strings");
    }
    strings.dump();
    
    if (args.ast) {
        if (args.to_separate_archives) {
            redirect_stdout("bla.ast");
        }
        p->print(0);
    }

    if (!args.to_separate_archives) {
        fprintf(stdout, "-------------------------------------------------------\n\n");
    }
    
    if ( !logger->exists_registered_error() ){
        p->gen_tac(b);
        if (args.to_separate_archives) {
            redirect_stdout("bla.tac");
        }
        b->dump(args.tac_with_comments);
    } else {
        cerr << "Epic fail!" << endl;
        abort();
    }

    if (!args.to_separate_archives) {
        fprintf(stdout, "-------------------------------------------------------\n\n");
    }

    b->gen_graph();
    delete_unreachable_code();

    redirect_stdout();
    cout << "Escribiendo bloques básico en bla.blocks"
         << " y grafo de flujo en bla.png\n\n";
    dump_in_file("bla");

    if (!args.to_separate_archives && !args.to_single_archive) {
        fprintf(stdout, "-------------------------------------------------------\n\n");
    }

    if ( logger->exists_registered_error() ){
        cerr << "Epic fail! (context)" << endl;
        return 0;
    }

    redirect_stdout();
    cout << "Like a boss!" << endl;

    return 0;
}