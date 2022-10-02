#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>

using namespace std;

template <typename T>
void printVector(vector<T> vec);

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

        // Função de Inicialização
        // Além de atribuir os elementos do AFD, como alfabetos, estados, etc. O minimaliza e mostra se
        // a linguagem reconhecida pelo AFD é vazia ou não
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

        // Analisa uma palavra, retorna se a palavra é aceitada ou rejeitada
        void AnalisaPalavra(string palavra)
        {
            // Analisa letra por letra da palavra, fazendo as transições correspondentes
            for(char c: palavra)
            {
                Transicao(c);

                if(Situacao == -2) break; // Se uma ultima transição for indefinido, para de analisar a palavra
            }

            Transicao(char(0)); // Transição da Palavra Vazia

            EA = i; // Reinicializa o estado atual para o inicial

            if(Situacao == 1) cout << "\n" << palavra << " ACEITA" << endl;
            else if(Situacao < 0) cout << "\n" << palavra << " REJEITADA" << endl;
        }

        // Dada uma lista de palavras, analisa cada um
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
                        Situacao = -1; AFD acabou e rejeitou
                        Situacao = -2; AFD não acabou mas Transição Indefinida
                        */

        vector<string> EstadosAlcancaveis; // Lista de estados alcançaveis, a partir do estado inicial
        vector<string> In_Estados_Al; // Lista de estados alcançaveis, a partir de um estado
                                      // Usado para saber se um estado é inútil ou não

        unordered_map<string, unordered_map<string, vector<string> > > TabelaE;
            // Tabela de equivalência
            /* Dado um par de estados, retorna um vetor de strings em que o primeiro elemento representa se aquele par é
               equivalente ou não, os outros elementos do vetor são os pares que são dependentes.
            */

        vector<string> emptyVector = {"X"}; // Vetor usado quando um par não é definido

        // Variaveis de Controle do teste de linguagem vazia
        unordered_map<string, bool> visitados; // Dado um estado, retorna se aquele estado foi visitado
        bool Vazia = true; // Se a linguagem é vazia

        // Função que faz uma transição do estado atual, dado um símbolo
        void Transicao(char c)
        {
            // Existe uma forma mais compacta mas c++ é uma linguagem horrível e fica dando problema
            // TESTA SE ENTRADA É VÁLIDA

                // Verifica se quando a palavra acaba, o estado atual é final ou não
                if(int(c) == 0)
                {
                    if(Situacao == -2) return; // Se tiver acabado por transição indefina, retorna

                    if (std::find(F.begin(), F.end(), EA) != F.end()) // Se o estado é final
                        Situacao = 1;
                    else // Ou não
                        Situacao = -1;
                    return;
                }

                Situacao = -2;
                // Tenta encontrar se o símbolo faz parte do alfabeto, existe um jeito melhor, talvez implemente depois
                for(char x: A){
                    if(x == c)
                    {
                        Situacao = 0; // Se for um símbolo válido, continua
                        break;
                    }
                }

                if(Situacao == -2) return;
            // ACABOU O TESTE :)


            try{
                EA = T.at(EA).at(c); // Tenta fazer a transição
            } catch (const std::out_of_range& e){
                Situacao = -1; // Se não der certo, retorna que a palavra não é aceita
            }
        }

        // Imprime as informações essenciais do AFD
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

            // Transforma todas as transições indefinidas para o estado de lixo dump
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
            ApenasAlcancaveis(i); // Atualiza os Estados Alcançaveis a partir do estado inicial

            // Passa por todos os estado do AFD, e não for Alcançavel, o exclui
            for(string Estado: S)
                if(std::find(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado) == EstadosAlcancaveis.end())
                    Remove_Estado(Estado);

//----------Etapas da Minimização-------------------------------------------------------------------------
            // Teste de equivalência de estados-----------------------------------------------------------

                //Inicializar a Tabela;
                // Vai por toda a tabela de equivalência e verifica se o par são ambos do conjuntos dos finais,
                // e um for e o outro não, ele marca com um X
            for(unsigned int i = 0; i<EstadosAlcancaveis.size()-1; i++)
            {
                for(unsigned int j = 0; j<=i; j++)
                {
                    bool p_Is = std::find(F.begin(), F.end(), EstadosAlcancaveis[j]) != F.end();
                    bool q_Is = std::find(F.begin(), F.end(), EstadosAlcancaveis[i+1]) != F.end();

                    if(p_Is != q_Is)
                        TabelaE[EstadosAlcancaveis[j]][EstadosAlcancaveis[i+1]] = {"X"};
                    else
                        TabelaE[EstadosAlcancaveis[j]][EstadosAlcancaveis[i+1]] = {" "};
                }
            }

            // Analisa os pares de estados que ainda não foram marcados, e verifica se são equivalentes
            bool flag = true;
            while(flag)
            {
                flag = false;

                for(unsigned int i = 0; i<EstadosAlcancaveis.size()-1; i++)
                {
                    for(unsigned int j = 0; j<=i; j++)
                    {
                        string p = EstadosAlcancaveis[j];
                        string q = EstadosAlcancaveis[i+1];

                        if(TabelaS(p,q)[0] == "X")
                            continue;

                        //VerificaPar(p,q);

                        for(char c: A)
                        {
                            string r = T[p][c];
                            string s = T[q][c];

                            if(r == s) continue;

                            bool marcado;
                            if(TabelaS(r,s)[0] == "X") marcado = true;
                            else marcado = false;

                            flag = flag || marcado;
                            TabelaS(p,q)[0] = TabelaS(r,s)[0];

                            if(marcado) break;
                        }
                    }
                }
            }

            // Unificação de estados equivalentes----------------------------------------------------------
            vector<string> Removidos;
            vector<string> Unidos;

            // Se depois da verifição, alguma par de estados ainda não tenham sido marcados
            // significa que são equivalentes, portanto podem ser unificados
            for(unsigned int ic = 0; ic<EstadosAlcancaveis.size()-1; ic++)
            {
                for(unsigned int j = 0; j<=ic; j++)
                {
                    string p = EstadosAlcancaveis[j];
                    string q = EstadosAlcancaveis[ic+1];
                    if(TabelaS(p,q)[0] == "X")
                        continue;

                    string pq = p + q; // Cria um novo estado a partir das partes

                    // Cria todas suas transições baseadas em uma de suas parte, qual parte é não importa, pois são equivalentes
                    for(char a: A)
                        T[pq][a] = T[p][a];

                    // Passa por todas as transições, e qualquer transições que seja direcionada a alguma das partes é redirecionada
                    // para o estado unificado
                    for(auto& i: T)
                        for(auto& j: i.second)
                            if(j.second == p || j.second == q) j.second = pq;

                    // Se uma das partes for o estado inicial, faz com que o estado unificado seja também
                    if(p == i || q == i)
                    {
                        i = pq;
                        EA = pq;
                    }

                    Unidos.push_back(pq);
                    Removidos.push_back(p);
                    Removidos.push_back(q);

                    // Se uma das partes for um estado final, o estado unificado será também
                    if(std::find(F.begin(), F.end(), p) != F.end() || std::find(F.begin(), F.end(), q) != F.end())
                        F.push_back(pq);
                }
            }

            // Remove os estados que foram unificados
            for(string p: Removidos)
            {
                EstadosAlcancaveis.erase(std::remove(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), p), EstadosAlcancaveis.end());
                T.erase(p);

                if(std::find(F.begin(), F.end(), p) != F.end())
                    F.erase(std::remove(F.begin(), F.end(), p), F.end());
            }

            // Adiciona os estados unificados
            EstadosAlcancaveis.insert( EstadosAlcancaveis.end(), Unidos.begin(), Unidos.end() );

            // Exclusão de estado inúteis----------------------------------------------------------
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

            // Remover as transições para estados não existentes----------------------------------------------------------
            unordered_map<string, vector<char>> A_Remover;
            for(auto& i: T)
            {
                for(auto& j: i.second)
                {
                    if(j.second == "") A_Remover[i.first].push_back(j.first);
                }
            }

            for(auto& i: A_Remover)
            {
                for(char c: i.second)
                {
                    cout << i.first << ',' << c << endl;
                    T[i.first].erase(c);
                }
            }

            A_Remover.clear();
        }

        // Passa por todos os estados alcançáveis a partir do Estado, e os guarda no vetor EstadosAlcancaveis
        void ApenasAlcancaveis(string Estado)
        {
            // Se o Estado já foi visitado, para
            if(find(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado) != EstadosAlcancaveis.end()) return;
            EstadosAlcancaveis.push_back(Estado); // Se não, adiciona na lista de estados alcançáveis

            // Remove Duplicatas
            vector<string>::iterator ip;
            ip = std::unique(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end());
            EstadosAlcancaveis.resize(std::distance(EstadosAlcancaveis.begin(), ip));
            string temp;

            // Repete os mesmo processo para os estados alcançaveis
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

        // E o mesmo processo do ApenasAlcancaveis, a única diferença é que guarda os estados alcançáveis no vetor In_Estados_Al
        // Para a exclusão dos estados Inuteis
        void ApenasInuteis(string Estado)
        {
            // Se o Estado já foi visitado, para
            if(find(In_Estados_Al.begin(), In_Estados_Al.end(), Estado) != In_Estados_Al.end()) return;
            In_Estados_Al.push_back(Estado); // Se não, adiciona na lista de estados alcançáveis

            // Remove Duplicatas
            vector<string>::iterator ip;
            ip = std::unique(In_Estados_Al.begin(), In_Estados_Al.end());
            In_Estados_Al.resize(std::distance(In_Estados_Al.begin(), ip));

            // Repete os mesmo processo para os estados alcançaveis
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

        // Remove um estado da lista de estados alcançaveis e da lista de transição
        void Remove_Estado(string Estado)
        {
            // Remove o estado das lista de Estados Alcançaveis
            EstadosAlcancaveis.erase(std::remove(EstadosAlcancaveis.begin(), EstadosAlcancaveis.end(), Estado), EstadosAlcancaveis.end());

            // Remove o estado na tabela de transição
            T.erase(Estado);

            // Remove as transição que contém o Estado
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

        // Verifica se o Par é equivalente, se dependerem de outros pares, os adicionam naquela lista de dependência
        void VerificaPar(string p, string q)
        {
            string parRaw = p + "," + q;

            // Passa por todas as transições possíveis
            for(char a: A)
            {
                string r = T[p][a];
                string s = T[q][a];

                if(r == s) continue; // Se os dois estados transicionam para o mesmo estado, então podem ser equivalente, não faz nada

                // Se o par r,s não estiver marcado, adicionar o par p,q como dependêntes
                if(TabelaS(r,s)[0] == " ")
                {
                    TabelaS(r,s).push_back(parRaw);
                } else if(TabelaS(r,s)[0] == "X") // Se o par r,s estiver marcado, marca o par p,q e suas dependências
                {
                    Marca(p,q);
                }
            }
        }

        // Marca o par p,q na tabela de equivalências, e todas as suas dependências
        void Marca(string p, string q)
        {
            // Marca p,q e se não tiver dependências, para
            TabelaS(p,q)[0] = "X";
            if(TabelaS(p,q).size() == 1)
                return;

            // Passa por todas as dependências, as marcando, de forma recursiva
            for(string par: range(TabelaS(p,q).begin()+1, TabelaS(p,q).end()))
            {
                vector<string> vec = divPar(par);
                string r = vec[0];
                string s = vec[1];

                if(TabelaS(p,q) == TabelaS(r,s)) continue;
                Marca(r,s);
            }
        }

        // Dado um par p e q, retorna como o endereço do par como ele está na tabela de equivalência, independente da ordem fornecida
        // se o par não constar na tabela, retorna um vector marcado
        vector<string> &TabelaS(string p, string q)
        {
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

        // Divide uma string "%s,%s" em um vetor de string {%s,%s}
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

        // Atualiza e verifica se a linguagem reconhecida pelo autômato é vazia
        void LinguagemVazia(void)
        {
            TesteVazia(i);

            if(Vazia)
                std::cout << "Linguagem Vazia!";
        }

        // A partir do estado e, começa a navegar o AFD até achar um estado final ou, se não achar, até o fim
        void TesteVazia(string e)
        {
            visitados[e] = true;

            if(!Vazia) return;

            if((std::find(F.begin(), F.end(), e) != F.end()) && Vazia)
            {
                std::cout << "Linguagem nao vazia!" << std::endl;
                Vazia = false;
                return;
            }

            for(char c: A)
            {
                string temp;
                try{
                    temp = T.at(e).at(c);
                } catch (const std::out_of_range& e){
                    continue;
                }

                if(visitados[temp]) continue;

                // Se a transição leva a um estado final, sinaliza que a linguagem reconhecida não é vazia
                if((std::find(F.begin(), F.end(), temp) != F.end()) && Vazia)
                {
                    std::cout << "Linguagem nao vazia!" << std::endl;
                    Vazia = false;
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

    cout << "Arquivo com lista de palavras: ";
    cin >> arq_entrada;
    myAFD.AnalisaLista(WordsFile(arq_entrada));

    cout << endl << "Escreva algo e de Enter pra sair" << endl;
    cin >> arq_entrada;
    return 0;
}

template <typename T>
void printVector(vector<T> vec)
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
    while(getline(arq_in, buffer, '\n'))
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
