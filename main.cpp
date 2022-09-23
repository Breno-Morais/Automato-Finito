#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>

using namespace std;

void printVector(vector<auto> vec);
template <typename T>
struct range_t
{
    T b, e;
    range_t(T x, T y) : b(x), e(y) {}
    T begin()
    {
        return b;
    }
    T end()
    {
        return e;
    }
};

template <typename T>
range_t<T> range(T b, T e)
{
    return range_t<T>(b, e);
}

class AFD
{
    public:
        string M; // Nome do AFD
        vector<string> S; // Conjunto dos estados do AFD
        vector<char> A; // Alfabeto do AFD
        string i; // Estado inicial
        vector<string> F; // Conjunto dos estados finais do AFD
        unordered_map<string, unordered_map<char, string>> T; // Tabela de Transição do AFD (Recebe um Estado e um Simbolo, retorna um Estado)

        string EA; // Estado Atual do AFD

        AFD(string NomeM, vector<string> estadosS, vector<char> alfabetosA, string estadoI, vector<string> estadosF, unordered_map<string, unordered_map<char, string>> TabelaEntrada)
        {
            M = NomeM;
            S = estadosS;
            A = alfabetosA;
            i = estadoI;
            F = estadosF;

            EA = estadoI;

            T = TabelaEntrada;

            Situacao = 0;

            EstadoAtual();
            Minimizacao();
            std::cout << "-------------- " << M << " Minimizado --------------" << std::endl;
            EstadoAtual();

            LinguagemVazia();
        }

        void AnalisaPalavra(string palavra)
        {
            for(char c: palavra)
            {
                Transicao(c);

                if(Situacao == -2) break;
            }
            Transicao(char(0));

            EA = i;

            if(Situacao == 1) cout << "\n" << palavra << " ACEITA" << endl;
            else if(Situacao < 0) cout << "\n" << palavra << " REJEITADA" << endl;
        }

        void AnalisaLista(vector<string> palavrasVector)
        {
            for(string palavra: palavrasVector)
            {
                AnalisaPalavra(palavra);
            }
        }

    private:
        int Situacao = 0; /*
                        Situação final do AFD
                        Situacao = 0; AFD não acabou
                        Situacao = 1; AFD acabou e aceitou
                        Situacao = -1; AFD acabou e rejeitou */
        vector<string> EstadosAlcancaveis;
        vector<string> In_Estados_Al;
        unordered_map<string, unordered_map<string, vector<string> > > TabelaE;
        unordered_map<string, bool> visitados;

        // Variaveis de Controle do teste de linguagem vazia
        bool Vazia = true;
        bool fim = false;

        void Transicao(char c)
        {
            // Existe uma forma mais compacta mais c++ é uma língua horrível e fica dando problema
            // TESTA SE ENTRADA É VÁLIDA

                // Verifica se quando a palavra acaba, o estado atual é final ou não
                if(int(c) == 0)
                {
                    if(Situacao == -2) return;
                    if (std::find(F.begin(), F.end(), EA) != F.end())
                        Situacao = 1;
                    else
                        Situacao = -1;
                    return;
                }

                Situacao = -2;
                for(char x: A){
                    if(x == c)
                    {
                        Situacao = 0;
                        break;
                    }
                }

                if(Situacao == -2) return;
            // ACABOU O TESTE :)


            try{
                EA =T.at(EA).at(c);
            } catch (const std::out_of_range& e){
                Situacao = -1;
            }
        }

        void EstadoAtual()
        {
            std::cout << "Nome AFD: " << M << endl;
            std::cout << "S: "; printVector(S);
            std::cout << "A: "; printVector(A);
            std::cout << "F: "; printVector(F);
            std::cout << "Estado Atual: " << EA << endl;
            std::cout << "Transicoes: " << endl;
            for(auto const& i: T)
            {
                for(auto const& j: i.second)
                {
                    std::cout << '(' << i.first << ',' << j.first << ',' << j.second << ')' << endl;
                }
            }
            std::cout << endl;
        }

        void Minimizacao()
        {
//----------Requisitos------------------------------------------------------------------------------------
            // Função Total
            string dump = "dumping";
            EstadosAlcancaveis.push_back(dump);
            for(string q: EstadosAlcancaveis)
            {
                for(char c: A)
                {
                    try{
                        string temp = T.at(q).at(c);
                    } catch (const std::out_of_range& e){
                        T[q][c] = dump;
                    }
                }
            }

            // Somente Estados Alcançaveis
            ApenasAlcancaveis(i);

            for(string Estado: S)
                if(std::find(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado) == EstadosAlcancaveis.end())
                    Remove_Estado(Estado);

//----------Etapas da Minimização-------------------------------------------------------------------------
            // Teste de equivalência de estados

                //Inicializar a Tabela;
                // Vai por toda a tabela de equivalência e verifica se o par são ambos do conjuntos dos finais,
                // e um for e o outro não, ele marca com um X
            for(int i = 0; i<EstadosAlcancaveis.size()-1; i++)
            {
                for(int j = 0; j<=i; j++)
                {
                    //std::cout << EstadosAlcancaveis[j] << ',' << EstadosAlcancaveis[i+1] << endl;
                    bool p_Is = std::find(F.begin(), F.end(), EstadosAlcancaveis[j]) != F.end();
                    bool q_Is = std::find(F.begin(), F.end(), EstadosAlcancaveis[i+1]) != F.end();

                    if(p_Is != q_Is)
                        TabelaE[EstadosAlcancaveis[j]][EstadosAlcancaveis[i+1]] = {"X"};
                    else
                        TabelaE[EstadosAlcancaveis[j]][EstadosAlcancaveis[i+1]] = {" "};
                }
            }

            for(int i = 0; i<EstadosAlcancaveis.size()-1; i++)
            {
                for(int j = 0; j<=i; j++)
                {
                    string p = EstadosAlcancaveis[j];
                    string q = EstadosAlcancaveis[i+1];
                    if(TabelaS(p,q)[0] == "X")
                        continue;

                    VerificaPar(p,q);
                }
            }

            // Unificação de estados equivalentes
            vector<string> Removidos;
            vector<string> Unidos;

            for(int ic = 0; ic<EstadosAlcancaveis.size()-1; ic++)
            {
                for(int j = 0; j<=ic; j++)
                {
                    string p = EstadosAlcancaveis[j];
                    string q = EstadosAlcancaveis[ic+1];
                    if(TabelaS(p,q)[0] == "X")
                        continue;

                    string pq = p + q;
                    for(char a: A)
                        T[pq][a] = T[p][a];

                    for(auto& i: T)
                        for(auto& j: i.second)
                            if(j.second == p || j.second == q) j.second = pq;

                    if(p == i || q == i)
                    {
                        i = pq;
                        EA = pq;
                    }

                    Unidos.push_back(pq);
                    Removidos.push_back(p);
                    Removidos.push_back(q);

                    if(std::find(F.begin(), F.end(), p) != F.end() || std::find(F.begin(), F.end(), q) != F.end())
                        F.push_back(pq);
                }
            }

            for(string p: Removidos)
            {
                EstadosAlcancaveis.erase(std::remove(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), p), EstadosAlcancaveis.end());
                T.erase(p);

                if(std::find(F.begin(), F.end(), p) != F.end())
                    F.erase(std::remove(F.begin(), F.end(), p), F.end());
            }

            EstadosAlcancaveis.insert( EstadosAlcancaveis.end(), Unidos.begin(), Unidos.end() );

            // Exclusão de estado inúteis
            Removidos.clear(); // Vou reutilizar o Removidos para guardar os estado inuteis
                // Ir para cada estado
            for(string Estado: EstadosAlcancaveis)
            {
                if(std::find(F.begin(), F.end(), Estado) != F.end()) continue;
                bool Inutil = true;

                In_Estados_Al.clear();
                ApenasInuteis(Estado);

                //  Verificar se algum estado alcançavei por ele é final
                for(string Estado2: In_Estados_Al)
                    if(std::find(F.begin(), F.end(), Estado2) != F.end()) Inutil = false;

                //  Se nenhum for final, adiciona a lista de estados a serem removidos
                if(Inutil)
                    Removidos.push_back(Estado);
            }
                // Remove os estados Inuteis
            for(string Estado: Removidos)
                Remove_Estado(Estado);

            // Construção do AF minimizado
            S = EstadosAlcancaveis;
            M = M + " Minimizado";

            // Remover as transições para estados não existentes
            unordered_map<string, char> A_Remover;
            for(auto& i: T)
            {
                for(auto& j: i.second)
                {
                    if(j.second == "") A_Remover[i.first] = j.first;
                }
            }

            for(auto& i: A_Remover)
            {
                T[i.first].erase(i.second);
            }

            A_Remover.clear();
        }

        void ApenasInuteis(string Estado)
        {
            if(find(In_Estados_Al.begin(), In_Estados_Al.end(), Estado) != In_Estados_Al.end()) return;

            In_Estados_Al.push_back(Estado);

            // Remove Duplicatas
            vector<string>::iterator ip;
            ip = std::unique(In_Estados_Al.begin(), In_Estados_Al.end());
            In_Estados_Al.resize(std::distance(In_Estados_Al.begin(), ip));
            string temp;

            for(char a: A)
            {
                try{
                    temp = T.at(Estado).at(a);
                } catch (const std::out_of_range& e){
                    continue;
                }
                ApenasInuteis(temp);
            }
        }

        void Remove_Estado(string Estado)
        {
            // Remove o estado das lista de Estados Alcançaveis
            EstadosAlcancaveis.erase(std::remove(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado), EstadosAlcancaveis.end());

            // Remove o estado na tabela de transição
            T.erase(Estado);

            unordered_map<string, char> A_Remover;
            for(auto& i: T)
            {
                for(auto& j: i.second)
                {
                    if(j.second == Estado) A_Remover[i.first] = j.first;
                }
            }

            for(auto& i: A_Remover)
            {
                T[i.first].erase(i.second);
            }

            A_Remover.clear();
        }

        void ApenasAlcancaveis(string Estado)
        {
            if(find(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado) != EstadosAlcancaveis.end()) return;

            EstadosAlcancaveis.push_back(Estado);

            // Remove Duplicatas
            vector<string>::iterator ip;
            ip = std::unique(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end());
            EstadosAlcancaveis.resize(std::distance(EstadosAlcancaveis.begin(), ip));
            string temp;

            for(char a: A)
            {
                try{
                    temp = T.at(Estado).at(a);
                } catch (const std::out_of_range& e){
                    continue;
                }
                ApenasAlcancaveis(temp);
            }
        }

        void VerificaPar(string p, string q)
        {
            string parRaw = p + "," + q;

            for(char a: A)
            {
                string r = T[p][a];
                string s = T[q][a];

                if(r == s) continue;

                if(TabelaS(r,s)[0] == " ")
                {
                    TabelaS(r,s).push_back(parRaw);
                } else if(TabelaS(r,s)[0] == "X")
                {
                    Marca(p,q);
                }
            }
        }

        void Marca(string p, string q)
        {
            TabelaS(p,q)[0] = "X";
            if(TabelaS(p,q).size() == 1)
                return;

            for(string par: range(TabelaS(p,q).begin()+1, TabelaS(p,q).end()))
            {
                vector<string> vec = divPar(par);
                string r = vec[0];
                string s = vec[1];

                if(TabelaS(p,q) == TabelaS(r,s)) continue;
                Marca(r,s);
            }
        }

        vector<string> &TabelaS(string p, string q)
        {
            vector<string> emptyVector = {"X"};
            try{
                return TabelaE.at(p).at(q);
            } catch (const std::out_of_range& e){
                try{
                    return TabelaE.at(q).at(p);
                } catch (const std::out_of_range& e){
                    return emptyVector;
                }
            }
        }

        vector<string> divPar(string parRaw)
        {
            vector<string> vecBuffer;
            string buffer;
            stringstream X(parRaw);
            getline(X,buffer,',');
            vecBuffer.push_back(buffer);
            getline(X,buffer);
            vecBuffer.push_back(buffer);
            return vecBuffer;
        }

        void LinguagemVazia(void)
        {
            TesteVazia(i);

            if(Vazia)
                std::cout << "Linguagem Vazia!";
        }

        void TesteVazia(string e)
        {
            visitados[e] = true;

            for(char c: A)
            {
                string temp = T[e][c];
                if(visitados[temp]) return;
                if((std::find(F.begin(), F.end(), temp) != F.end()) && Vazia)
                {
                    std::cout << "Linguagem nao vazia!" << std::endl;
                    Vazia = false;
                    fim = true;
                    return;
                }
                TesteVazia(temp);
            }
        }
};

AFD AFDFile(string ArqNome);
vector<string> WordsFile(string ArqNome);

int main()
{
    string arq_entrada;
    cout << "Nome do arquivo AFD: ";
    cin >> arq_entrada;
    AFD myAFD = AFDFile(arq_entrada);

    /*
    cout << "Arquivo com lista de palavras: ";
    cin >> arq_entrada;
    myAFD.AnalisaLista(WordsFile(arq_entrada));
    */

    return 0;
}

void printVector(vector<auto> vec)
{
    std::cout << "( ";
    for(auto i: vec)
        std::cout << i << ' ';
    std::cout << ')' << endl;
}

vector<string> WordsFile(string ArqNome)
{
    ifstream arq_in(ArqNome);

    vector<string> words;

    string buffer;
    while(getline(arq_in, buffer))
        words.push_back(buffer);

    arq_in.close();

    return words;
}

AFD AFDFile(string ArqNome)
{
    ifstream arq_in(ArqNome);

    string NomeAFD; // Nome do AFD
    vector<string> EstadosAFD; // Conjunto dos estados do AFD
    vector<char> Alfabeto; // Alfabeto do AFD
    string EstadoInicial; // Estado inicial
    vector<string> Finais; // Conjunto dos estados finais do AFD
    unordered_map<string, unordered_map<char, string>> TabelaTransicao; // Tabela de Transição do AFD

    string buffer;
    size_t pos = 0;

    // NOME
    getline(arq_in,NomeAFD);

    // CONJUNTO DE ESTADOS
    getline(arq_in, buffer, ' '); // Lê e pula o "S: "
    getline(arq_in, buffer, '\n'); // Lê todos os estados até o fim da linha
    while((pos = buffer.find(',')) != std::string::npos)
    {
        string statebuffer = buffer.substr(0, pos);
        EstadosAFD.push_back(statebuffer);
        buffer.erase(0, pos + 1);
    }

    if(buffer.length() > 0)
    {
        EstadosAFD.push_back(buffer);
    }

    // ALFABETO
    getline(arq_in, buffer, ' '); // Lê e pula o "S: "
    getline(arq_in, buffer, '\n'); // Lê todos os estados até o fim da linha
    while((pos = buffer.find(',')) != std::string::npos)
    {
        string statebuffer = buffer.substr(0, pos);
        Alfabeto.push_back(statebuffer[0]);
        buffer.erase(0, pos + 1);
    }

    if(buffer.length() > 0)
    {
        Alfabeto.push_back(buffer[0]);
    }

    // ESTADO INICIAL
    getline(arq_in, buffer, ' '); // Lê e pula o "S: "
    getline(arq_in, buffer, '\n'); // Lê todos os estados até o fim da linha
    EstadoInicial = buffer;

    // ESTADOS FINAIS
    getline(arq_in, buffer, ' '); // Lê e pula o "S: "
    getline(arq_in, buffer, '\n'); // Lê todos os estados até o fim da linha
    while((pos = buffer.find(',')) != std::string::npos)
    {
        string statebuffer = buffer.substr(0, pos);
        Finais.push_back(statebuffer);
        buffer.erase(0, pos + 1);
    }

    if(buffer.length() > 0)
    {
        Finais.push_back(buffer);
    }

    // TABELA DE TRANSIÇÕES
    string EsTraIn; // Estado de Transição Inicial
    string buffercT; // Ele não pega caracteres, só strings
    char cT; // Simbolo de Transição
    string EsTraFi; // Estado de Transição Final

    while(getline(arq_in, buffer, '('))
    {
        getline(arq_in, EsTraIn, ',');
        getline(arq_in, buffercT, ',');
        getline(arq_in, EsTraFi, ')');
        cT = buffercT[0];

        TabelaTransicao[EsTraIn][cT] = EsTraFi;
    }
    arq_in.close();

    AFD tempAFD(NomeAFD,EstadosAFD,Alfabeto,EstadoInicial,Finais, TabelaTransicao);

    return tempAFD;
}
