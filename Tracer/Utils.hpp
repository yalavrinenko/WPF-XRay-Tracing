//
// Created by yalavrinenko on 15.01.19.
//

#ifndef XRAY_TRACING_LIB_UTILS_HPP
#define XRAY_TRACING_LIB_UTILS_HPP

#include <vector>
#include <mutex>
#include <cstdio>

template <class TLogData>
class XRTFLogging{
public:
    XRTFLogging() = default;

    explicit XRTFLogging(std::string const &path):
        log_path(path){
        open(log_path);
    }

    void header(std::string const &header_data) {
        if (logptr)
            std::fprintf(logptr, "%s\n", header_data.c_str());
    }

    void flush(){
        if (logptr) {
            for (auto i=0u; i < index; ++i){
                auto data = buffer[i].to_string();
                fprintf(logptr, "%s\n", data.c_str());
            }
            index = 0;
        }
    }

    void add_data(TLogData const &data){
        if (logptr) {
            buffer[index] = data;
            ++index;

            if (buffer.capacity() == index)
                flush();
        }
    }

    bool is_open() const {
        return logptr != nullptr;
    }

    ~XRTFLogging(){
        if (!logptr)
            return;

        this->flush();
        std::fclose(logptr);
    }

protected:
    void open(std::string const &path){
        if (path.empty()) {
            logptr = nullptr;
            return;
        }

        logptr = fopen(this->log_path.c_str(), "w");
        if (!logptr)
            throw std::runtime_error("Unable to open file:" + log_path);

        buffer.resize(default_capacity);
        index = 0;
    }

private:
    using xrt_logbuffer = std::vector<TLogData>;
    const size_t default_capacity = 1000;

    std::string log_path;
    FILE* logptr = nullptr;

    xrt_logbuffer buffer;
    size_t index = 0;
};

template <class TData, class TStorage = std::vector<TData>>
class threadsafe_vector{
public:
    threadsafe_vector() = default;

    explicit threadsafe_vector(size_t limits = 0):
        capacity((limits) ? limits : default_capacity), m_storage(capacity){
    }

    void add_data(TData const &value){
        std::lock_guard<std::mutex> lg(write_mutex);
        m_storage[iter] = value;
        ++iter;
    }

    void reset() {
        std::lock_guard<std::mutex> lg(write_mutex);
        iter = 0;
    }

    TStorage const& container() const{
        return m_storage;
    }

private:
    const size_t default_capacity = 1000;

    std::mutex write_mutex;

    size_t capacity;
    size_t iter{0};
    TStorage m_storage;
};

#endif //XRAY_TRACING_LIB_UTILS_HPP
