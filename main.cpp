#include <any>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <unicode/fmtable.h>
#include <unicode/msgfmt.h>
#include <unicode/unistr.h>
#include <unicode/utypes.h>

[[nodiscard]] std::string toStdString(const icu::UnicodeString &icuString) {
  std::string output;
  icuString.toUTF8String(output);
  return output;
}

[[nodiscard]] icu::UnicodeString toIcuString(const std::string &stdString) {
  return icu::UnicodeString::fromUTF8(stdString);
}

using Formattable =
    std::variant<double, int32_t, int64_t, const char *, std::string>;

template <typename T> icu::Formattable toIcuFormattable(const T &value) {
  return {value};
}

template <>
icu::Formattable toIcuFormattable<std::string>(const std::string &value) {
  return toIcuString(value);
}

template <typename... F>
[[nodiscard]] std::vector<icu::Formattable> getArgumentValues(F... arguments) {
  std::vector<icu::Formattable> values;
  (values.push_back(toIcuFormattable(arguments)), ...);
  return values;
}

// helper type for the visitor #4
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

[[nodiscard]] std::vector<icu::Formattable> getArgumentValues(
    const std::unordered_map<std::string, Formattable> &arguments) {
  std::vector<icu::Formattable> values;

  for (const auto &[key, value] : arguments) {
    values.push_back(
        std::visit([](auto &&v) { return toIcuFormattable(v); }, value));
  }

  return values;
}

std::vector<icu::UnicodeString> getArgumentNames(
    const std::unordered_map<std::string, Formattable> &placeholders) {
  std::vector<icu::UnicodeString> names;

  std::transform(std::begin(placeholders), std::end(placeholders),
                 std::back_inserter(names),
                 [](auto &&keyValue) { return toIcuString(keyValue.first); });

  return names;
}

[[nodiscard]] std::string
formatX(std::string input,
        std::unordered_map<std::string, Formattable> placeholders) {
  const auto argNames = getArgumentNames(placeholders);
  const auto argValues = getArgumentValues(placeholders);

  UErrorCode error = UErrorCode::U_ZERO_ERROR;
  const icu::MessageFormat format{toIcuString(input), error};
  icu::UnicodeString resultUnicode;
  format.format(argNames.data(), argValues.data(), argValues.size(),
                resultUnicode, error);

  if (U_FAILURE(error))
    return u_errorName(error);

  return toStdString(resultUnicode);
}

template <typename... PositionalPlaceholders>
[[nodiscard]] std::string format(std::string input,
                                 PositionalPlaceholders... placeholders) {
  const auto argValues = getArgumentValues(placeholders...);

  UErrorCode error = UErrorCode::U_ZERO_ERROR;
  const icu::MessageFormat format{toIcuString(input), error};
  icu::UnicodeString resultUnicode;
  icu::FieldPosition fieldPosition;
  format.format(argValues.data(), argValues.size(), resultUnicode,
                fieldPosition, error);

  if (U_FAILURE(error))
    return u_errorName(error);

  return toStdString(resultUnicode);
}

int main() {
  std::unordered_map<std::string, Formattable> x = {
      {"username", "foo"},
  };

  std::cout << "Hello, World!" << std::endl;
  std::cout << formatX("Hello, {username}! {bar, number, #}",
                       {
                           {"username", "foo"},
                           {"bar", 12345},
                       })
            << std::endl;
  std::cout << format("Hello, {0}! {1}", 1, "y") << std::endl;

  std::cout
      << format("{foo, plural, zero {empty} one {an foo} other {{foo} foos}}",
                2)
      << std::endl;

  return 0;
}
