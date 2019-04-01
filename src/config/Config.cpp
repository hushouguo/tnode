/*
 * \file: Config.cpp
 * \brief: Created by hushouguo at 15:29:40 Mar 25 2019
 */

#include "tnode.h"

BEGIN_NAMESPACE_TNODE {
	bool Config::loadconf(const char* xmlfile) {
		XmlParser xmlParser;
		if (!xmlParser.open(xmlfile)) {
			fprintf(stderr, "loadconf: %s failure\n", xmlfile);
			return false;
		}

		this->clear();
		xmlParser.makeRegistry(this);
		xmlParser.final();
		//this->dump();
		return true;			
	}

	bool Config::init(int argc, char* argv[]) {
		int c;
		/*--argc, argv++;*/
		while ((c = getopt(argc, argv, "dDhHc:gt:")) != -1) {
			switch (c) {
				case 'd': case 'D': sConfig.runasdaemon = true; break;
				case 'c': this->confile = optarg; break;
				case 'g': this->guard = true; break;
				case 't': this->threads = atoi(optarg); break;
				case 'h': case 'H': default: 
						  fprintf(stderr, "Usage: tnode [OPTIONS]\n");
						  fprintf(stderr, "    OPTIONS:\n");
						  fprintf(stderr, "      -d: 			run as daemon, default: %s\n", sConfig.runasdaemon ? "true" : "false");
						  fprintf(stderr, "      -c filename: load config file, default: %s\n", sConfig.confile.c_str());
						  fprintf(stderr, "      -g: 			enable guard process, default: %s\n", sConfig.guard ? "true" : "false");
						  fprintf(stderr, "      -t: number: special init number of thread, default: %d\n", sConfig.threads);
						  return false;
			}
		}

		setlocale(LC_ALL, "");// for chinese console output

		if (this->runasdaemon) {
			daemon(1, 1); /* nochdir, noclose */
			if (this->guard) {
				while (!this->halt) {
					int pid = fork();
					if (pid) {
						setProcesstitle(argc, argv, " guard");
						int status = 0;
						waitpid(pid, &status, 0);
						if (status == 1) {
							fprintf(stderr, "childProcess status: %d on exit\n", status);
							::exit(0);	// status == 1 means childProcess exit on init stage
						}
						fprintf(stderr, "Guard find childProcess exit with status: %d\n", status);
						resetProcesstitle(argc, argv);
					}
					else { break; }
				}
			}
		}

		if (!this->confile.empty()) {
			this->confile = absoluteDirectory(this->confile.c_str());
		}
		return this->confile.empty() ? true : this->loadconf(this->confile.c_str());
	}

	INITIALIZE_INSTANCE(Config);
}
