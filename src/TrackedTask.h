#ifndef MIKROSKOP_TRACKEDTASK_H
#define MIKROSKOP_TRACKEDTASK_H

#include <atomic>
#include <thread>
#include "Logger.h"

class TrackedTask {
  std::thread         m_thread;
  std::atomic< bool > m_running;
  std::atomic< bool > m_finished;
  std::atomic< bool > m_overlapping;

public:
  TrackedTask(const TrackedTask&)            = delete;
  TrackedTask& operator=(const TrackedTask&) = delete;

  TrackedTask(): m_running(false), m_finished(false), m_overlapping(false) {
  }

  template< typename Fn, typename... Args >
  explicit TrackedTask(bool overlapping, Fn&& task, Args&&... args):
    m_overlapping(overlapping) {
    m_running = false;
    m_finished = false;
    m_thread = std::thread(
      [this](Fn&& task, Args&&... args) { // TODO: clang-tidy: ??
        tracked(std::forward< Fn >(task), std::forward< Args >(args)...);
      },
      std::forward< Fn >(task),
      std::forward< Args >(args)...);
  }

  ~TrackedTask() = default;

  TrackedTask(TrackedTask&& other) noexcept:
    m_thread(std::move(other.m_thread)),
    m_running(other.m_running.load()),
    m_finished(other.m_finished.load()),
    m_overlapping(other.m_overlapping.load()) {
  }

  TrackedTask& operator=(TrackedTask&& other) noexcept {
    if (this != &other)
    {
      if (!m_overlapping)
      {
        safeJoin();
      } else if (!m_finished) {
        m_thread.detach();
      }

      m_running  = other.m_running.load();
      m_finished = other.m_finished.load();
      m_overlapping = other.m_overlapping.load();
      m_thread   = std::move(other.m_thread);
    }
    return *this;
  }

  inline void safeJoin() {
    if (m_thread.joinable()) { m_thread.join(); }
  }

  [[nodiscard]] inline bool isRunning() const noexcept {
    return m_running;
  }

  [[nodiscard]] inline bool isFinished() const noexcept {
    return m_finished;
  }

private:
  template< class Fn, class... Args >
  constexpr auto tracked(Fn task, Args&&... args) {
    m_running = true;
    task(std::forward< Args >(args)...);
    m_running  = false;
    m_finished = true;
  }
};

#endif // MIKROSKOP_TRACKEDTASK_H
