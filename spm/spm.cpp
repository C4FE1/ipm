#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <map>


std::map<std::string, std::string> configs();
std::map<std::string, std::string> config = configs();

std::string work_dir = "/tmp/spm/";
std::string repo_url = config["Repo"];

std::map<std::string, std::string> configs(){	
	std::map<std::string, std::string> configs_list;
   	
	// Abre o arquivo em modo de leitura
    	std::ifstream arquivo("spm.conf");

    	// Verifica se o arquivo foi aberto corretamente
    	if (arquivo.is_open()) {
        
		std::string linha;

        	// Lê cada linha do arquivo
        	while (std::getline(arquivo, linha)) {
        
			// Encontra a posição do caractere '=' na linha
            		size_t pos = linha.find('=');

            		// Verifica se o caractere '=' foi encontrado
            		if (pos != std::string::npos) {
              			configs_list[linha.substr(0, pos)] = linha.substr(pos + 1);
            		}		
        	}

        	// Fecha o arquivo
       		arquivo.close();
    	
	} else {
        	std::cerr << "Erro in the opening of config file." << std::endl;
    	}

    	return configs_list;	
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::ofstream* file) {
    size_t total_size = size * nmemb;
    file->write(static_cast<char*>(contents), total_size);
    return total_size;
}

void request_file(const std::string& url, const std::string& filename) {
    CURL* curl;
    CURLcode res;
    std::ofstream outputFile(filename, std::ios::binary);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputFile);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

void install_only(const std::string& repo_url, const std::string& pkg_name){
	const std::string pkg_url = repo_url + pkg_name;
	const std::string pkg_path = work_dir + pkg_name;
	request_file(pkg_url,pkg_path);
	system(("chmod +x " + pkg_path).c_str());
	system((pkg_path + " " + "i").c_str());
}

void remove_only(const std::string& pkg_name){
	const std::string pkg_path = work_dir + pkg_name;
	system(("chmod +x " + pkg_path).c_str());
	system((pkg_path + " " + "r").c_str());
}

int main(int argc,char* argv[]){
	if(argv[1] == config["Install (only)"]){
		repo_url = config["Repo"];
		install_only(repo_url,argv[2]);	
	}else if(argv[1] == config["Remove (only)"] && argc >= 3){
		remove_only(argv[2]);
	}else{
		std::cout << "numtendi" << std::endl;
	}
	
	return 0;
}
