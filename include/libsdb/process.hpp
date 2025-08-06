#ifndef SDB_PROCESS_HPP
#define SDB_PROCESS_HPP

#include <filesystem> 
#include <memory> 
#include <sys/types.h> 
#include <cstdint> 

namespace sdb {

    enum class process_state {
        stopped, 
        running, 
        exited,
        terminated
    };

    struct stop_reason {
        stop_reason(int wait_status);

        process_state reason; 
        std::uint8_t info;
    };


    class process {
    public:
        process() = delete;
        process(const process&) = delete;
        process& operator=(const process&) = delete;
        ~process();
    
        static std::unique_ptr<process> launch(std::filesystem::path path);
        static std::unique_ptr<process> attach(pid_t pid);

        void resume(); 
        stop_reason wait_on_signal();

        pid_t pid() const { return m_pid; }
        process_state state() const { return m_state; }
        
    private: 
        pid_t m_pid = 0;
        bool m_terminate_on_end = true;
        process_state m_state = process_state::stopped;

        process(pid_t pid, bool terminate_on_end): m_pid(pid), m_terminate_on_end(terminate_on_end) {}
    };
}

#endif 