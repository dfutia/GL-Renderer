#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>
#include <string>
#include <unordered_map>

struct BenchmarkResult
{
    float lastTime = 0.0f;      // microseconds
    float avgTime = 0.0f;
    float minTime = FLT_MAX;
    float maxTime = 0.0f;
    int sampleCount = 0;
};

class Benchmarker
{
public:
    static Benchmarker& Instance()
    {
        static Benchmarker instance;
        return instance;
    }

    void Record(const std::string& name, float microseconds)
    {
        auto& r = results[name];
        r.lastTime = microseconds;
        r.minTime = std::min(r.minTime, microseconds);
        r.maxTime = std::max(r.maxTime, microseconds);
        r.sampleCount++;

        // Running average
        r.avgTime += (microseconds - r.avgTime) / r.sampleCount;
    }

    void Reset(const std::string& name) { results.erase(name); }
    void ResetAll() { results.clear(); }

    const std::unordered_map<std::string, BenchmarkResult>& GetResults() const { return results; }

private:
    std::unordered_map<std::string, BenchmarkResult> results;
};

class ScopedTimer
{
public:
    ScopedTimer(const std::string& name)
        : name(name), start(std::chrono::high_resolution_clock::now())
    {
    }

    ~ScopedTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        Benchmarker::Instance().Record(name, static_cast<float>(duration.count()));
    }

private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
};

#define BENCHMARK_SCOPE(name) ScopedTimer _timer##__LINE__(name)

#endif