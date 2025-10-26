/**
 * @file expected.hpp
 * @brief A C++17 implementation of std::expected (similar to C++23)
 * 
 * This provides expected<T, E> for representing values or errors.
 * Follows the C++23 std::expected API but implemented for C++17.
 */

#ifndef PROJECT_EXPECTED_HPP_
#define PROJECT_EXPECTED_HPP_

#include <exception>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

namespace project
{
  /**
   * @brief Exception thrown when accessing value of an expected containing an error
   */
  template<typename E>
  class bad_expected_access : public std::exception
  {
  private:
    E error_;

  public:
    explicit bad_expected_access(E error) : error_(std::move(error))
    {
    }

    const E& error() const& noexcept
    {
      return error_;
    }

    E& error() & noexcept
    {
      return error_;
    }

    const char* what() const noexcept override
    {
      return "bad expected access";
    }
  };

  /**
   * @brief Tag type for constructing the error part of expected
   */
  struct unexpect_t
  {
    explicit unexpect_t() = default;
  };

  inline constexpr unexpect_t unexpect{};

  /**
   * @brief Wrapper for error values in expected
   */
  template<typename E>
  class unexpected
  {
  private:
    E error_;

  public:
    constexpr unexpected(const unexpected&) = default;
    constexpr unexpected(unexpected&&) = default;

    template<typename Err = E>
    constexpr explicit unexpected(Err&& e) : error_(std::forward<Err>(e))
    {
    }

    constexpr const E& error() const& noexcept
    {
      return error_;
    }

    constexpr E& error() & noexcept
    {
      return error_;
    }

    constexpr const E&& error() const&& noexcept
    {
      return std::move(error_);
    }

    constexpr E&& error() && noexcept
    {
      return std::move(error_);
    }
  };

  template<typename E>
  unexpected(E) -> unexpected<E>;

  /**
   * @brief A type that contains either a value or an error
   * 
   * expected<T, E> is a vocabulary type that contains either a value of type T
   * or an error of type E. This is useful for error handling without exceptions.
   * 
   * @tparam T The type of the expected value
   * @tparam E The type of the error
   */
  template<typename T, typename E>
  class expected
  {
  private:
    std::variant<T, E> data_;

  public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    // Constructors
    constexpr expected() : data_(T{})
    {
    }

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;

    template<typename U = T>
    constexpr explicit(!std::is_convertible_v<U, T>) expected(U&& v) : data_(std::in_place_index<0>, std::forward<U>(v))
    {
    }

    template<typename G>
    constexpr explicit(!std::is_convertible_v<const G&, E>) expected(const unexpected<G>& e)
        : data_(std::in_place_index<1>, e.error())
    {
    }

    template<typename G>
    constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G>&& e)
        : data_(std::in_place_index<1>, std::move(e.error()))
    {
    }

    template<typename... Args>
    constexpr explicit expected(std::in_place_t, Args&&... args) : data_(std::in_place_index<0>, std::forward<Args>(args)...)
    {
    }

    template<typename... Args>
    constexpr explicit expected(unexpect_t, Args&&... args) : data_(std::in_place_index<1>, std::forward<Args>(args)...)
    {
    }

    // Assignment
    constexpr expected& operator=(const expected&) = default;
    constexpr expected& operator=(expected&&) = default;

    template<typename U = T>
    constexpr expected& operator=(U&& v)
    {
      data_.template emplace<0>(std::forward<U>(v));
      return *this;
    }

    template<typename G>
    constexpr expected& operator=(const unexpected<G>& e)
    {
      data_.template emplace<1>(e.error());
      return *this;
    }

    template<typename G>
    constexpr expected& operator=(unexpected<G>&& e)
    {
      data_.template emplace<1>(std::move(e.error()));
      return *this;
    }

    // Observers
    constexpr explicit operator bool() const noexcept
    {
      return has_value();
    }

    constexpr bool has_value() const noexcept
    {
      return data_.index() == 0;
    }

    constexpr const T& value() const&
    {
      if (!has_value())
      {
        throw bad_expected_access(std::get<1>(data_));
      }
      return std::get<0>(data_);
    }

    constexpr T& value() &
    {
      if (!has_value())
      {
        throw bad_expected_access(std::get<1>(data_));
      }
      return std::get<0>(data_);
    }

    constexpr const T&& value() const&&
    {
      if (!has_value())
      {
        throw bad_expected_access(std::move(std::get<1>(data_)));
      }
      return std::move(std::get<0>(data_));
    }

    constexpr T&& value() &&
    {
      if (!has_value())
      {
        throw bad_expected_access(std::move(std::get<1>(data_)));
      }
      return std::move(std::get<0>(data_));
    }

    constexpr const E& error() const&
    {
      return std::get<1>(data_);
    }

    constexpr E& error() &
    {
      return std::get<1>(data_);
    }

    constexpr const E&& error() const&&
    {
      return std::move(std::get<1>(data_));
    }

    constexpr E&& error() &&
    {
      return std::move(std::get<1>(data_));
    }

    constexpr const T* operator->() const noexcept
    {
      return &std::get<0>(data_);
    }

    constexpr T* operator->() noexcept
    {
      return &std::get<0>(data_);
    }

    constexpr const T& operator*() const& noexcept
    {
      return std::get<0>(data_);
    }

    constexpr T& operator*() & noexcept
    {
      return std::get<0>(data_);
    }

    constexpr const T&& operator*() const&& noexcept
    {
      return std::move(std::get<0>(data_));
    }

    constexpr T&& operator*() && noexcept
    {
      return std::move(std::get<0>(data_));
    }

    template<typename U>
    constexpr T value_or(U&& default_value) const&
    {
      return has_value() ? std::get<0>(data_) : static_cast<T>(std::forward<U>(default_value));
    }

    template<typename U>
    constexpr T value_or(U&& default_value) &&
    {
      return has_value() ? std::move(std::get<0>(data_)) : static_cast<T>(std::forward<U>(default_value));
    }
  };

  /**
   * @brief Specialization for void value type
   */
  template<typename E>
  class expected<void, E>
  {
  private:
    std::variant<std::monostate, E> data_;

  public:
    using value_type = void;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    constexpr expected() : data_(std::in_place_index<0>)
    {
    }

    constexpr expected(const expected&) = default;
    constexpr expected(expected&&) = default;

    template<typename G>
    constexpr explicit(!std::is_convertible_v<const G&, E>) expected(const unexpected<G>& e)
        : data_(std::in_place_index<1>, e.error())
    {
    }

    template<typename G>
    constexpr explicit(!std::is_convertible_v<G, E>) expected(unexpected<G>&& e)
        : data_(std::in_place_index<1>, std::move(e.error()))
    {
    }

    constexpr expected& operator=(const expected&) = default;
    constexpr expected& operator=(expected&&) = default;

    constexpr explicit operator bool() const noexcept
    {
      return has_value();
    }

    constexpr bool has_value() const noexcept
    {
      return data_.index() == 0;
    }

    constexpr void value() const
    {
      if (!has_value())
      {
        throw bad_expected_access(std::get<1>(data_));
      }
    }

    constexpr const E& error() const&
    {
      return std::get<1>(data_);
    }

    constexpr E& error() &
    {
      return std::get<1>(data_);
    }

    constexpr const E&& error() const&&
    {
      return std::move(std::get<1>(data_));
    }

    constexpr E&& error() &&
    {
      return std::move(std::get<1>(data_));
    }
  };

}  // namespace project

#endif  // PROJECT_EXPECTED_HPP_

