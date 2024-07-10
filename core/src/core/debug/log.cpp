#include "log.h"

namespace playchilla {

log& logger() {
	static log log;
	return log;
}

log& dummy_log() {
	static log log(false);
	return log;
}

log& plog() {
	static thread_local log plog(false);
	return plog;
}

}
