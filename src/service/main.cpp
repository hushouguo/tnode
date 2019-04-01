/*
 * \file: main.cpp
 * \brief: Created by hushouguo at 15:10:52 Mar 25 2019
 */

#include "tnode.h"

using namespace tnode;

bool init_runtime_environment(int argc, char* argv[]) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	//
	// parser command line arguments
	//
	if (!sConfig.init(argc, argv)) { return false; }

	//
	// config Easylog
	//
	Easylog::syslog()->set_level((EasylogSeverityLevel) sConfig.get("log.level", GLOBAL));
	Easylog::syslog()->set_autosplit_day(sConfig.get("log.autosplit_day", true));
	Easylog::syslog()->set_autosplit_hour(sConfig.get("log.autosplit_hour", false));
	Easylog::syslog()->set_destination(sConfig.get("log.dir", ".logs"));
	Easylog::syslog()->set_tofile(GLOBAL, getProgramName());
	Easylog::syslog()->set_tostdout(GLOBAL, sConfig.runasdaemon ? false : true);


	//
	// tnode
	//
#ifdef DEBUG		
	Trace.cout("tnode: %d.%d.%d, threads: %d, run as %s, %s, debug", TNODE_VERSION_MAJOR, TNODE_VERSION_MINOR, TNODE_VERSION_PATCH, sConfig.threads, sConfig.runasdaemon ? "daemon" : "console", sConfig.guard ? "with guard" : "no guard");
#else		
	Trace.cout("tnode: %d.%d.%d, threads: %d, run as %s, %s, release", TNODE_VERSION_MAJOR, TNODE_VERSION_MINOR, TNODE_VERSION_PATCH, sConfig.threads, sConfig.runasdaemon ? "daemon" : "console", sConfig.guard ? "with guard" : "no guard");
#endif

	//
	// Config information
	//
	if (sConfig.confile.empty()) {
		Alarm << "specify config file: Unspecified";
	}
	else {
		Trace << "specify config file: " << sConfig.confile;
	}
	sConfig.dump();

#if 0
	//
	// Easylog configure information
	//
	extern const char* tnode::level_string(EasylogSeverityLevel);
	Trace.cout("Easylog:");
	Trace.cout("    log.level: %s", level_string(Easylog::syslog()->level()));
	Trace.cout("    log.autosplit_day: %s, log.autosplit_hour: %s", 
			Easylog::syslog()->autosplit_day() ? "yes" : "no", 
			Easylog::syslog()->autosplit_hour() ? "yes" : "no");
	Trace.cout("    log.dir: %s", Easylog::syslog()->destination());
#endif

	//
	// limit
	//
	size_t stack_size = sConfig.get("limit.stack_size", 0u);
	if (stack_size > 0) {
		setStackSizeLimit(stack_size);
	}

	size_t max_files = sConfig.get("limit.max_files", 0u);
	if (max_files > 0) {
		setOpenFilesLimit(max_files);
	}

	Trace.cout("stack size: %u (limit.stack_size), max files: %u (limit.max_files)", getStackSizeLimit(), getOpenFilesLimit());

	//
	// install signal handler
	//
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_INTERRUPT; //The system call that is interrupted by this signal will not be restarted automatically
	act.sa_handler = [](int sig) {
		//fprintf(stderr, "receive signal: %d\n", sig);
		// Don't call Non reentrant function, just like malloc, free etc, i/o function also cannot call.
		if (sig == SIGRTMIN) {		// SIGRTMIN: Wake up thread, nothing to do
			return;	// SIGRTMIN: #define SIGRTMIN        (__libc_current_sigrtmin ())
		}
		switch (sig) {
			case SIGWINCH: break;	// the window size change, ignore
			case SIGHUP: sConfig.reload = true;	// NOTE: reload configure file						   
			case SIGALRM: break;	// timer expire
			default: sConfig.syshalt(sig); break;
		}
	};

#if 0
	sigaction(SIGHUP, &act, nullptr);		// 1
	sigaction(SIGINT, &act, nullptr);		// 2
	sigaction(SIGQUIT, &act, nullptr);		// 3
	sigaction(SIGILL, &act, nullptr);		// 4
	sigaction(SIGTRAP, &act, nullptr);		// 5
	sigaction(SIGABRT, &act, nullptr);		// 6
	sigaction(SIGIOT, &act, nullptr);		// 6
	sigaction(SIGBUS, &act, nullptr);		// 7
	sigaction(SIGFPE, &act, nullptr);		// 8
	// 9 => SIGKILL
	sigaction(SIGUSR1, &act, nullptr);		// 10
	sigaction(SIGSEGV, &act, nullptr);		// 11
	sigaction(SIGUSR2, &act, nullptr);		// 12
	sigaction(SIGPIPE, &act, nullptr);		// 13
	sigaction(SIGALRM, &act, nullptr);		// 14
	sigaction(SIGTERM, &act, nullptr);		// 15
	sigaction(SIGSTKFLT, &act, nullptr); 	// 16
	// 17 => SIGCHLD
	sigaction(SIGCONT, &act, nullptr);		// 18
	// 19 => SIGSTOP
	sigaction(SIGTSTP, &act, nullptr);		// 20
	sigaction(SIGTTIN, &act, nullptr);		// 21
	sigaction(SIGTTOU, &act, nullptr);		// 22
	sigaction(SIGURG, &act, nullptr);		// 23
	sigaction(SIGXCPU, &act, nullptr);		// 24
	sigaction(SIGXFSZ, &act, nullptr);		// 25
	sigaction(SIGVTALRM, &act, nullptr); 	// 26
	sigaction(SIGPROF, &act, nullptr);		// 27
	sigaction(SIGWINCH, &act, nullptr);		// 28
	sigaction(SIGIO, &act, nullptr); 		// 29
	sigaction(SIGPWR, &act, nullptr);		// 30
	sigaction(SIGSYS, &act, nullptr);		// 31
	sigaction(SIGRTMIN, &act, nullptr);		// 34
#endif

	//
	// output 3rd libraries
	//
	Trace.cout("all 3rd libraries:");

#ifdef TC_VERSION_MAJOR		
	Trace.cout("    tcmalloc: %d.%d%s", TC_VERSION_MAJOR, TC_VERSION_MINOR, TC_VERSION_PATCH);
#else
	Trace.cout("    not link tcmalloc");
#endif

#ifdef LIBEVENT_VERSION
	Trace.cout("    libevent: %s", LIBEVENT_VERSION);
#endif

#ifdef ZMQ_VERSION_MAJOR
	Trace.cout("    libzmq: %d.%d.%d", ZMQ_VERSION_MAJOR, ZMQ_VERSION_MINOR, ZMQ_VERSION_PATCH);
#endif

#ifdef LUAJIT_VERSION
	Trace.cout("    luaJIT: %s -- %s", LUAJIT_VERSION, LUAJIT_COPYRIGHT);
#endif

#ifdef GOOGLE_PROTOBUF_VERSION
	Trace.cout("    protobuf: %d, library: %d", GOOGLE_PROTOBUF_VERSION, GOOGLE_PROTOBUF_MIN_LIBRARY_VERSION);
#endif

	Trace.cout("    rapidxml: 1.13");

#ifdef MYSQL_SERVER_VERSION		
	Trace.cout("    mysql: %s", MYSQL_SERVER_VERSION);
#endif

	Trace.cout("    gcc version: %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);

	return true;
}

//
// shutdown routine
void shutdown() {
	//NOTE: cleanup internal resource
	Trace.cout("shutdown system with terminate reason: %d", sConfig.terminate_reason);
	Easylog::syslog()->stop();
	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();
}

int main(int argc, char* argv[]) {
	if (!init_runtime_environment(argc, argv)) { return 1; }

	sThreadPool.init(sConfig.threads);
	CHECK_GOTO(sServiceManager.newservice(sConfig.get("tnode.entryfile", "N/A")), exit_failure, "ServiceManager init failure");

	while (!sConfig.halt) {
		sTime.now();
		sServiceManager.schedule();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}	

exit_failure:
	sServiceManager.stop();
	sThreadPool.stop();
	shutdown();
	return 0;	
}

