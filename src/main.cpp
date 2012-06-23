
#include "main.h"
#include "../y.tab.c"

void main_read_args(int argc, char **argv, FILE *in, main_args& args);

void main_redirect_stdout(const char *f = 0);

AST* main_ast_create(main_args args);

void main_ast_check(AST *p);

Tac* main_tac_create(main_args args, AST *p);

void main_tac_optimize(Tac* tac);

void main_tac_dump(main_args args, Tac* tac);

int main (int argc,char **argv)
{
    main_args args;
    main_read_args(argc, argv, stdin, args);

    logger = new llog();
    AST *p = main_ast_create(args);
    
    main_ast_check(p);
    
    if (args.to_separate_archives) {
        main_redirect_stdout("bla.types");
    }
    types.dump(cout);
    cout << "-------------------------------------------------------" << endl << endl;
    if (args.to_separate_archives) {
        main_redirect_stdout("bla.strings");
    }
    strings.dump(cout);
    cout << "-------------------------------------------------------" << endl << endl;
    
    if (args.ast) {
        if (args.to_separate_archives) {
            main_redirect_stdout("bla.ast");
        }
        p->dump(cout, 0);

        if (!args.to_separate_archives) {
            cout << "-------------------------------------------------------" << endl << endl;
        }
    }
    
    if ( logger->exists_registered_error() ){
        logger->dump(cerr);
        cerr << "-------------------------------------------------------" << endl << endl;
        cerr << "Epic fail!" << endl;
        abort();
        return 1;
    }
    
    Tac *tac = main_tac_create(args, p);

    main_tac_optimize(tac);

    main_tac_dump(args, tac);

    main_redirect_stdout();
    cout << "Like a boss!" << endl;

    return 0;
}

void main_read_args(int argc, char **argv, FILE *in, main_args& args)
{
    args.to_single_archive = false;
    args.to_separate_archives = false;
    args.ast = false;
    args.tac_with_comments = false;
    args.in = in;
    if (1 == argc) {
        cout << "Opciones para " << argv[0] << " [-a | -s] [-c] [-t] archivo:" << endl
             << " -a imprimir en archivo bla.output" << endl
             << " -s imprimir en archivo bla.* separados" << endl
             << " -c imprimir el tac con comentarios" << endl
             << " -t imprimir el ast" << endl;
        exit(0);
    }
    for (int i = 1; i < argc; ++i) {
        if (i + 1 == argc) {
            args.in = fopen(argv[i], "r");
            if (!args.in) {
                cout << "Archivo '" << argv[i] << "' no existe" << endl;
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
            cout << "Opciones para " << argv[0] << " [-a | -s] [-c] [-t] archivo:" << endl
                 << " -a imprimir en archivo bla.output" << endl
                 << " -s imprimir en archivo bla.* separados" << endl
                 << " -c imprimir el tac con comentarios" << endl
                 << " -t imprimir el ast" << endl;
            exit(1);
        }
    }
}

AST* main_ast_create(main_args args){

    p = new AST();
    yyin = args.in;
    do {
        yyparse();
    } while (!feof(yyin));

    if (args.to_single_archive){
        main_redirect_stdout("bla.output");
    }
    return p;
}

void main_ast_check(AST *p){

    symbol_table* st = new symbol_table();
    
    p->fill_and_check(st);
    
    delete st;
}

Tac* main_tac_create(main_args args, AST *p){

    Tac* tac = p->to_tac();
    if (args.to_separate_archives) {
        main_redirect_stdout("bla.tac");
    }
    tac->dump(cout, args.tac_with_comments);

    if (!args.to_separate_archives) {
        cout << "-------------------------------------------------------" << endl << endl;
    }

    return tac;
}

void main_tac_optimize(Tac* tac)
{
    tac->gen_graph();
    tac->optimize();
}

void main_tac_dump(main_args args, Tac* tac)
{
    main_redirect_stdout();
    cout << "Escribiendo bloques básico en bla.blocks"
         << " y grafo de flujo en bla.png\n\n";
    dump_in_file("bla");

    if (!args.to_separate_archives && !args.to_single_archive) {
        cout << "-------------------------------------------------------" << endl << endl;
    }
}

void main_redirect_stdout(const char *f)
{
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