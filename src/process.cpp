#include <libsdb/process.hpp>
#include <libsdb/error.hpp> 

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

std::unique_ptr<sdb::process> sdb::process::launch(std::filesystem::path path){
    pid_t pid; 
    if ((pid = fork()) < 0) {
        error::send_errno("fork failed"); 
    }

    if (pid == 0){
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0){
            error::send_errno("Tracing failed");
        }
        if (execlp(path.c_str(), path.c_str(), nullptr) < 1){
            error::send_errno("exec failed");
        }
    }

    std::unique_ptr<sdb::process> proc(new process(pid, true));
    proc->wait_on_signal();

    return proc;
}

std::unique_ptr<sdb::process> sdb::process::attach(pid_t pid){
    if (pid == 0){
        error::send("Invalid PID");
    }

    if (ptrace(PTRACE_ATTACH, pid, nullptr, nullptr) < 0){
        error::send_errno("Could not attach");
    }

    std::unique_ptr<sdb::process> proc(new process(pid, false));
    proc->wait_on_signal();

    return proc;
}

sdb::process::~process() {
    if (m_pid != 0) {
        int status;
        if (m_state == process_state::running) {
            kill(m_pid, SIGSTOP);
            waitpid(m_pid, &status, 0);
        }
        ptrace(PTRACE_DETACH, m_pid, nullptr, nullptr);
        kill(m_pid, SIGCONT);

        if (m_terminate_on_end) {
            kill(m_pid, SIGKILL);
            waitpid(m_pid, &status, 0);
        }
    }
}

void sdb::process::resume() {
    if (ptrace(PTRACE_CONT, m_pid, nullptr, nullptr) < 0) {
        error::send_errno("Could not resume");
    }
    m_state = process_state::running;
}

sdb::stop_reason::stop_reason(int wait_status){
    if (WIFEXITED(wait_status)){
        reason = process_state::exited;
        info = WEXITSTATUS(wait_status);
    }
    else if (WIFSIGNALED(wait_status)){
        reason = process_state::terminated;
        info = WTERMSIG(wait_status);
    }
    else if (WIFSTOPPED(wait_status)){
        reason = process_state::stopped; 
        info = WSTOPSIG(wait_status);
    }
}

sdb::stop_reason sdb::process::wait_on_signal() {
    int wait_status;
    int options = 0;
    if (waitpid(m_pid, &wait_status, options) < 0) {
        error::send_errno("waitpid failed");
    }
    stop_reason reason(wait_status);
    m_state = reason.reason;
    return reason;
}