
#include <tte/engine/engine.hpp>
#include <gtest/gtest.h>
#include <string>

static const char* empty_string = "";
static const char* string_1 = "string_1";
static const char* string_2 = "string_2_something_different";
static const char* string_3 = "string_3_something_different_again";
static const char* string_4 = "string_4_something_different_again_and_again";
static const char* string_5 = "string_5_something_different_again_and_again_and_again";

static tte::engine::Buffer& create_buffer(std::vector<std::string> lines) {
    tte::engine::Buffer& buffer = tte::engine::create_buffer();
    for (tte::Length i = 0; i < lines.size(); ++i) {
        tte::engine::insert_line(buffer, i, lines[i].c_str());
    }
    return buffer;
}

static void assert_buffer_state(tte::engine::Buffer& buffer, std::vector<std::string> lines) {
    ASSERT_EQ(tte::engine::get_buffer_length(buffer), lines.size());
    for (tte::Length i = 0; i < lines.size(); ++i) {
        ASSERT_EQ(tte::engine::get_line_length(buffer, i), lines[i].size());
        char* line_string = tte::engine::line_to_c_string(buffer, i);
        ASSERT_EQ(line_string, lines[i]);
        free(static_cast<void*>(line_string));
    }
}

// #region Buffer& create_buffer()
TEST(engine, createBuffer) {
    tte::engine::Buffer& buffer = tte::engine::create_buffer();
    ASSERT_EQ(tte::engine::get_buffer_length(buffer), 0);
}

TEST(engine, createBufferTwiceGivesDifferentBuffers) {
    tte::engine::Buffer& buffer1 = tte::engine::create_buffer();
    tte::engine::Buffer& buffer2 = tte::engine::create_buffer();
    ASSERT_NE(&buffer1, &buffer2);
}
// #endregion

// #region void destroy_buffer(Buffer&)
TEST(engine, destroyBuffer) {
    tte::engine::Buffer& buffer = tte::engine::create_buffer();
    tte::engine::destroy_buffer(buffer);
}
// #endregion

// #region bool insert_empty_line(Buffer&, Length line_index)
static void test_insert_empty_line(std::vector<std::string> lines,
    tte::Length line_index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::insert_empty_line(buffer, line_index), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, insertEmptyLineAtBeginningOfEmptyBuffer) { test_insert_empty_line({}, 0, {empty_string}, true); }

TEST(engine, insertEmptyLineAtInvalidIndexOfEmptyBuffer) { test_insert_empty_line({}, 1, {}, false); }

TEST(engine, insertEmptyLineAtBeginningOfNonEmptyBuffer) {
    test_insert_empty_line({string_1}, 0, {empty_string, string_1}, true);
}

TEST(engine, insertEmptyLineAtEndOfNonEmptyBuffer) {
    test_insert_empty_line({string_1}, 1, {string_1, empty_string}, true);
}

TEST(engine, insertEmptyLineAtMiddleOfNonEmptyBuffer) {
    test_insert_empty_line({string_1, string_2}, 1, {string_1, empty_string, string_2}, true);
}

TEST(engine, insertEmptyLineAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_empty_line({string_1}, 2, {string_1}, false);
}
// #endregion

// #region bool insert_line(Buffer&, Length line_index, Char* contents)
static void test_insert_line(std::vector<std::string> lines,
    tte::Length line_index,
    std::string line,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    {
        tte::engine::Buffer& buffer = create_buffer(lines);

        char* line_without_null_terminator = static_cast<char*>(malloc(sizeof(char) * line.size()));
        memcpy(line_without_null_terminator, line.c_str(), line.size());
        ASSERT_EQ(tte::engine::insert_line(buffer, line_index, line_without_null_terminator, line.size()),
            expected_result);
        assert_buffer_state(buffer, expected_lines);
    }

    {
        tte::engine::Buffer& buffer = create_buffer(lines);

        ASSERT_EQ(tte::engine::insert_line(buffer, line_index, line.c_str()), expected_result);
        assert_buffer_state(buffer, expected_lines);
    }
}

TEST(engine, insertLineAtBeginningOfEmptyBuffer) { test_insert_line({}, 0, string_1, {string_1}, true); }

TEST(engine, insertLineAtInvalidIndexOfEmptyBuffer) { test_insert_line({}, 1, string_1, {}, false); }

TEST(engine, insertLineAtBeginningOfNonEmptyBuffer) {
    test_insert_line({string_1}, 0, string_2, {string_2, string_1}, true);
}

TEST(engine, insertLineAtEndOfNonEmptyBuffer) { test_insert_line({string_1}, 1, string_2, {string_1, string_2}, true); }

TEST(engine, insertLineAtMiddleOfNonEmptyBuffer) {
    test_insert_line({empty_string, string_1}, 1, string_2, {empty_string, string_2, string_1}, true);
}

TEST(engine, insertLineAtInvalidIndexOfNonEmptyBuffer) { test_insert_line({string_1}, 2, string_2, {string_1}, false); }
// #endregion

// #region bool insert_empty_lines(Buffer&, Length number_of_lines, Length line_index);
static void test_insert_empty_lines(std::vector<std::string> lines,
    tte::Length number_of_lines,
    tte::Length line_index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::insert_empty_lines(buffer, number_of_lines, line_index), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, insertEmptyLinesZeroAtBeginningOfEmptyBuffer) { test_insert_empty_lines({}, 0, 0, {}, true); }

TEST(engine, insertEmptyLinesOneAtBeginningOfEmptyBuffer) { test_insert_empty_lines({}, 1, 0, {empty_string}, true); }

TEST(engine, insertEmptyLinesManyAtBeginningOfEmptyBuffer) {
    test_insert_empty_lines({}, 3, 0, {empty_string, empty_string, empty_string}, true);
}

TEST(engine, insertEmptyLinesZeroAtInvalidIndexOfEmptyBuffer) { test_insert_empty_lines({}, 0, 1, {}, true); }

TEST(engine, insertEmptyLinesOneAtInvalidIndexOfEmptyBuffer) { test_insert_empty_lines({}, 1, 1, {}, false); }

TEST(engine, insertEmptyLinesManyAtInvalidIndexOfEmptyBuffer) { test_insert_empty_lines({}, 3, 1, {}, false); }

TEST(engine, insertEmptyLinesZeroAtBeginningOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 0, 0, {string_1, string_2}, true);
}

TEST(engine, insertEmptyLinesOneAtBeginningOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 1, 0, {empty_string, string_1, string_2}, true);
}

TEST(engine, insertEmptyLinesManyAtBeginningOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2},
        3,
        0,
        {empty_string, empty_string, empty_string, string_1, string_2},
        true);
}

TEST(engine, insertEmptyLinesZeroAtMiddleOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 0, 1, {string_1, string_2}, true);
}

TEST(engine, insertEmptyLinesOneAtMiddleOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 1, 1, {string_1, empty_string, string_2}, true);
}

TEST(engine, insertEmptyLinesManyAtMiddleOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2},
        3,
        1,
        {string_1, empty_string, empty_string, empty_string, string_2},
        true);
}

TEST(engine, insertEmptyLinesZeroAtEndOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 0, 2, {string_1, string_2}, true);
}

TEST(engine, insertEmptyLinesOneAtEndOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 1, 2, {string_1, string_2, empty_string}, true);
}

TEST(engine, insertEmptyLinesManyAtEndOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2},
        3,
        2,
        {string_1, string_2, empty_string, empty_string, empty_string},
        true);
}

TEST(engine, insertEmptyLinesZeroAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 0, 3, {string_1, string_2}, true);
}

TEST(engine, insertEmptyLinesOneAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 1, 3, {string_1, string_2}, false);
}

TEST(engine, inserEmptyLinestManyAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_empty_lines({string_1, string_2}, 3, 3, {string_1, string_2}, false);
}
// #endregion

// #region bool insert_lines(Buffer&, const Length number_of_lines, const Length line_index, const Char** data_array,
// const Length* data_length_array);
static void test_insert_lines(std::vector<std::string> lines,
    tte::Length line_index,
    std::vector<std::string> lines_to_insert,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    const std::size_t number_of_lines = lines_to_insert.size();
    char** c_lines = static_cast<char**>(malloc(sizeof(char*) * number_of_lines));
    for (std::size_t i = 0; i < lines_to_insert.size(); ++i) {
        std::size_t line_length = lines_to_insert[i].size();
        c_lines[i] = static_cast<char*>(malloc(sizeof(char) * (line_length + 1)));
        memcpy(c_lines[i], lines_to_insert[i].c_str(), line_length);
        c_lines[i][line_length] = '\0';
    }
    ASSERT_EQ(tte::engine::insert_lines(buffer, number_of_lines, line_index, const_cast<const char**>(c_lines)),
        expected_result);
    assert_buffer_state(buffer, expected_lines);
    for (std::size_t i = 0; i < lines_to_insert.size(); ++i) {
        free(c_lines[i]);
    }
    free(c_lines);
}

TEST(engine, insertLinesZeroAtBeginningOfEmptyBuffer) { test_insert_lines({}, 0, {}, {}, true); }

TEST(engine, insertLinesOneAtBeginningOfEmptyBuffer) { test_insert_lines({}, 0, {string_1}, {string_1}, true); }

TEST(engine, insertLinesManyAtBeginningOfEmptyBuffer) {
    test_insert_lines({}, 0, {string_1, string_2, string_3}, {string_1, string_2, string_3}, true);
}

TEST(engine, insertLinesZeroAtInvalidIndexOfEmptyBuffer) { test_insert_lines({}, 1, {}, {}, true); }

TEST(engine, insertLinesOneAtInvalidIndexOfEmptyBuffer) { test_insert_lines({}, 1, {string_1}, {}, false); }

TEST(engine, insertLinesManyAtInvalidIndexOfEmptyBuffer) {
    test_insert_lines({}, 1, {string_1, string_2, string_3}, {}, false);
}

TEST(engine, insertLinesZeroAtBeginningOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 0, {}, {string_1, string_2}, true);
}

TEST(engine, insertLinesOneAtBeginningOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 0, {string_3}, {string_3, string_1, string_2}, true);
}

TEST(engine, insertLinesManyAtBeginningOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2},
        0,
        {string_3, string_4, string_5},
        {string_3, string_4, string_5, string_1, string_2},
        true);
}

TEST(engine, insertLinesZeroAtMiddleOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 1, {}, {string_1, string_2}, true);
}

TEST(engine, insertLinesOneAtMiddleOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 1, {string_3}, {string_1, string_3, string_2}, true);
}

TEST(engine, insertLinesManyAtMiddleOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2},
        1,
        {string_3, string_4, string_5},
        {string_1, string_3, string_4, string_5, string_2},
        true);
}

TEST(engine, insertLinesZeroAtEndOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 2, {}, {string_1, string_2}, true);
}

TEST(engine, insertLinesOneAtEndOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 2, {string_3}, {string_1, string_2, string_3}, true);
}

TEST(engine, insertLinesManyAtEndOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2},
        2,
        {string_3, string_4, string_5},
        {string_1, string_2, string_3, string_4, string_5},
        true);
}

TEST(engine, insertLinesZeroAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 3, {}, {string_1, string_2}, true);
}

TEST(engine, insertLinesOneAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 3, {string_3}, {string_1, string_2}, false);
}

TEST(engine, insertLinesManyAtInvalidIndexOfNonEmptyBuffer) {
    test_insert_lines({string_1, string_2}, 3, {string_3, string_4, string_5}, {string_1, string_2}, false);
}
// #endregion

// #region void insert_character(Buffer&, const Length line_index, const Length character_index, const Char character);
static void test_insert_character(std::vector<std::string> lines,
    tte::Length line_index,
    tte::Length character_index,
    char character,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::insert_character(buffer, line_index, character_index, character), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, insertCharacterAtInvalidLineIndexOfEmptyBuffer) { test_insert_character({}, 3, 0, 'a', {}, false); }

TEST(engine, insertCharacterAtInvalidLineIndexOfNonEmptyBuffer) {
    test_insert_character({string_1}, 3, 0, 'a', {string_1}, false);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_character({empty_string, string_1, string_2}, 0, 1, 'a', {empty_string, string_1, string_2}, false);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_character({string_1, empty_string, string_2}, 1, 1, 'a', {string_1, empty_string, string_2}, false);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_character({string_1, string_2, empty_string}, 2, 1, 'a', {string_1, string_2, empty_string}, false);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_character({string_1, string_2, string_3},
        0,
        strlen(string_1) + 1,
        'a',
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_character({string_1, string_2, string_3},
        1,
        strlen(string_2) + 1,
        'a',
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_character({string_1, string_2, string_3},
        2,
        strlen(string_3) + 1,
        'a',
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_character({"", string_2, string_3}, 0, 0, 'a', {"a", string_2, string_3}, true);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_character({"hello", string_2, string_3}, 0, 0, 'a', {"ahello", string_2, string_3}, true);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_character({"hello", string_2, string_3}, 0, 1, 'a', {"haello", string_2, string_3}, true);
}

TEST(engine, insertCharacterAtBeginningOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_character({"hello", string_2, string_3}, 0, strlen("hello"), 'a', {"helloa", string_2, string_3}, true);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_character({string_1, "", string_3}, 1, 0, 'a', {string_1, "a", string_3}, true);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_character({string_1, "hello", string_3}, 1, 0, 'a', {string_1, "ahello", string_3}, true);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_character({string_1, "hello", string_3}, 1, 1, 'a', {string_1, "haello", string_3}, true);
}

TEST(engine, insertCharacterAtMiddleOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_character({string_1, "hello", string_3}, 1, strlen("hello"), 'a', {string_1, "helloa", string_3}, true);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_character({string_1, string_2, ""}, 2, 0, 'a', {string_1, string_2, "a"}, true);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_character({string_1, string_2, "hello"}, 2, 0, 'a', {string_1, string_2, "ahello"}, true);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_character({string_1, string_2, "hello"}, 2, 1, 'a', {string_1, string_2, "haello"}, true);
}

TEST(engine, insertCharacterAtEndOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_character({string_1, string_2, "hello"}, 2, strlen("hello"), 'a', {string_1, string_2, "helloa"}, true);
}
// #endregion

// #region void insert_characters(Buffer&, const Length line_index, const Length character_index, const Char* data);
static void test_insert_characters(std::vector<std::string> lines,
    tte::Length line_index,
    tte::Length character_index,
    std::string characters,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::insert_characters(buffer, line_index, character_index, characters.c_str()), expected_result);
    assert_buffer_state(buffer, expected_lines);
    tte::engine::destroy_buffer(buffer);
}

TEST(engine, insertCharactersZeroAtInvalidLineIndexOfEmptyBuffer) { test_insert_characters({}, 10, 0, "", {}, true); }

TEST(engine, insertCharactersOneAtInvalidLineIndexOfEmptyBuffer) { test_insert_characters({}, 10, 0, "a", {}, false); }

TEST(engine, insertCharactersManyAtInvalidLineIndexOfEmptyBuffer) {
    test_insert_characters({}, 10, 0, "abd", {}, false);
}

TEST(engine, insertCharactersZeroAtInvalidLineIndexOfNonEmptyBuffer) {
    test_insert_characters({string_1}, 10, 0, "", {string_1}, true);
}

TEST(engine, insertCharactersOneAtInvalidLineIndexOfNonEmptyBuffer) {
    test_insert_characters({string_1}, 10, 0, "a", {string_1}, false);
}

TEST(engine, insertCharactersManyAtInvalidLineIndexOfNonEmptyBuffer) {
    test_insert_characters({string_1}, 10, 0, "abd", {string_1}, false);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 1, "", {empty_string, string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 1, "a", {empty_string, string_2, string_3}, false);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 1, "abc", {empty_string, string_2, string_3}, false);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 1, "", {string_1, empty_string, string_3}, true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 1, "a", {string_1, empty_string, string_3}, false);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 1, "abc", {string_1, empty_string, string_3}, false);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 1, "", {string_1, string_2, empty_string}, true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 1, "a", {string_1, string_2, empty_string}, false);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 1, "abc", {string_1, string_2, empty_string}, false);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        0,
        strlen(string_1) + 1,
        "",
        {string_1, string_2, string_3},
        true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        0,
        strlen(string_1) + 1,
        "a",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        0,
        strlen(string_1) + 1,
        "abc",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        1,
        strlen(string_2) + 1,
        "",
        {string_1, string_2, string_3},
        true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        1,
        strlen(string_2) + 1,
        "a",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        1,
        strlen(string_2) + 1,
        "abc",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        2,
        strlen(string_3) + 1,
        "",
        {string_1, string_2, string_3},
        true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        2,
        strlen(string_3) + 1,
        "a",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtInvalidCharacterIndexOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        2,
        strlen(string_3) + 1,
        "abc",
        {string_1, string_2, string_3},
        false);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 0, "", {empty_string, string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 0, "a", {"a", string_2, string_3}, true);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({empty_string, string_2, string_3}, 0, 0, "abc", {"abc", string_2, string_3}, true);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3}, 0, 0, "", {string_1, string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        0,
        0,
        "a",
        {"a" + std::string(string_1), string_2, string_3},
        true);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        0,
        0,
        "abc",
        {"abc" + std::string(string_1), string_2, string_3},
        true);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 1, "", {"abc", string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 1, "z", {"azbc", string_2, string_3}, true);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 1, "123", {"a123bc", string_2, string_3}, true);
}

TEST(engine, insertCharactersZeroAtBeginningOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 3, "", {"abc", string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtBeginningOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 3, "z", {"abcz", string_2, string_3}, true);
}

TEST(engine, insertCharactersManyAtBeginningOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({"abc", string_2, string_3}, 0, 3, "123", {"abc123", string_2, string_3}, true);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 0, "", {string_1, empty_string, string_3}, true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 0, "a", {string_1, "a", string_3}, true);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, empty_string, string_3}, 1, 0, "abc", {string_1, "abc", string_3}, true);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3}, 1, 0, "", {string_1, string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        1,
        0,
        "a",
        {string_1, "a" + std::string(string_2), string_3},
        true);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        1,
        0,
        "abc",
        {string_1, "abc" + std::string(string_2), string_3},
        true);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 1, "", {string_1, "abc", string_3}, true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 1, "z", {string_1, "azbc", string_3}, true);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 1, "123", {string_1, "a123bc", string_3}, true);
}

TEST(engine, insertCharactersZeroAtMiddleOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 3, "", {string_1, "abc", string_3}, true);
}

TEST(engine, insertCharactersOneAtMiddleOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 3, "z", {string_1, "abcz", string_3}, true);
}

TEST(engine, insertCharactersManyAtMiddleOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, "abc", string_3}, 1, 3, "123", {string_1, "abc123", string_3}, true);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 0, "", {string_1, string_2, empty_string}, true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 0, "a", {string_1, string_2, "a"}, true);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtBeginningOfEmptyLine) {
    test_insert_characters({string_1, string_2, empty_string}, 2, 0, "abc", {string_1, string_2, "abc"}, true);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3}, 2, 0, "", {string_1, string_2, string_3}, true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        2,
        0,
        "a",
        {string_1, string_2, "a" + std::string(string_3)},
        true);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtBeginningOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, string_3},
        2,
        0,
        "abc",
        {string_1, string_2, "abc" + std::string(string_3)},
        true);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 1, "", {string_1, string_2, "abc"}, true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 1, "z", {string_1, string_2, "azbc"}, true);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtMiddleOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 1, "123", {string_1, string_2, "a123bc"}, true);
}

TEST(engine, insertCharactersZeroAtEndOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 3, "", {string_1, string_2, "abc"}, true);
}

TEST(engine, insertCharactersOneAtEndOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 3, "z", {string_1, string_2, "abcz"}, true);
}

TEST(engine, insertCharactersManyAtEndOfNonEmptyBufferAtEndOfNonEmptyLine) {
    test_insert_characters({string_1, string_2, "abc"}, 2, 3, "123", {string_1, string_2, "abc123"}, true);
}
// #endregion

// #region void delete_line(Buffer&, const Length line_index);
static void test_delete_line(std::vector<std::string> lines,
    tte::Length line_index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::delete_line(buffer, line_index), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, deleteLineAtInvalidLineIndexOfEmptyBuffer) { test_delete_line({}, 0, {}, false); }

TEST(engine, deleteLineAtInvalidLineIndexOfNonEmptyBuffer) {
    test_delete_line({string_1, string_2, string_3}, 5, {string_1, string_2, string_3}, false);
}

TEST(engine, deleteLineAtBeginningOfBufferWithOneLine) { test_delete_line({string_1}, 0, {}, true); }

TEST(engine, deleteLineAtBeginningOfBufferWithManyLines) {
    test_delete_line({string_1, string_2, string_3}, 0, {string_2, string_3}, true);
}

TEST(engine, deleteLineAtMiddleOfBufferWithManyLines) {
    test_delete_line({string_1, string_2, string_3}, 1, {string_1, string_3}, true);
}

TEST(engine, deleteLineAtEndOfBufferWithManyLines) {
    test_delete_line({string_1, string_2, string_3}, 2, {string_1, string_2}, true);
}
// #endregion

// #region void delete_lines(Buffer&, const Length number_of_lines, const Length line_index);
static void test_delete_lines(std::vector<std::string> lines,
    tte::Length number_of_lines,
    tte::Length index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);

    ASSERT_EQ(tte::engine::delete_lines(buffer, number_of_lines, index), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, deleteLinesZeroAtInvalidIndexOfEmptyBuffer) { test_delete_lines({}, 0, 0, {}, true); }

TEST(engine, deleteLinesOneAtInvalidIndexOfEmptyBuffer) { test_delete_lines({}, 1, 0, {}, false); }

TEST(engine, deleteLinesManyAtInvalidIndexOfEmptyBuffer) { test_delete_lines({}, 3, 0, {}, false); }

TEST(engine, deleteLinesZeroAtInvalidIndexOfNonEmptyBuffer) {
    test_delete_lines({string_1, string_2}, 0, 2, {string_1, string_2}, true);
}

TEST(engine, deleteLinesOneAtInvalidIndexOfNonEmptyBuffer) {
    test_delete_lines({string_1, string_2}, 1, 2, {string_1, string_2}, false);
}

TEST(engine, deleteLinesManyAtInvalidIndexOfNonEmptyBuffer) {
    test_delete_lines({string_1, string_2}, 3, 2, {string_1, string_2}, false);
}

TEST(engine, deleteLinesZeroAtBeginningOfBufferWithOneLine) { test_delete_lines({string_1}, 0, 0, {string_1}, true); }

TEST(engine, deleteLinesOneAtBeginningOfBufferWithOneLineWhereDeletionFitsExactly) {
    test_delete_lines({string_1}, 1, 0, {}, true);
}

TEST(engine, deleteLinesManyAtBeginningOfBufferWithOneLineWhereDeletionGoesOffTheEnd) {
    // TODO(TB): when the deletion goes past the end, should it fail and not change the buffer, or succeed and delete as
    // much as possible?
    test_delete_lines({string_1}, 3, 0, {}, true);
}

TEST(engine, deleteLinesZeroAtBeginningOfBufferWithManyLines) {
    test_delete_lines({string_1, string_2}, 0, 0, {string_1, string_2}, true);
}

TEST(engine, deleteLinesOneAtBeginningOfBufferWithManyLinesWhereDeletionFits) {
    test_delete_lines({string_1, string_2}, 1, 0, {string_2}, true);
}

TEST(engine, deleteLinesManyAtBeginningOfBufferWithManyLinesWhereDeletionGoesOffTheEnd) {
    // TODO(TB): when the deletion goes past the end, should it fail and not change the buffer, or succeed and delete as
    // much as possible?
    test_delete_lines({string_1, string_2}, 3, 0, {}, true);
}

TEST(engine, deleteLinesManyAtBeginningOfBufferWithManyLinesWhereDeletionFitsExactly) {
    test_delete_lines({string_1, string_2, string_3}, 3, 0, {}, true);
}

TEST(engine, deleteLinesManyAtBeginningOfBufferWithManyLinesWhereDeletionFits) {
    test_delete_lines({string_1, string_2, string_3, string_4}, 3, 0, {string_4}, true);
}

TEST(engine, deleteLinesZeroAtMiddleOfBufferWithManyLines) {
    test_delete_lines({string_1, string_2, string_3}, 0, 1, {string_1, string_2, string_3}, true);
}

TEST(engine, deleteLinesOneAtMiddleOfBufferWithManyLinesWhereDeletionFits) {
    test_delete_lines({string_1, string_2, string_3}, 1, 1, {string_1, string_3}, true);
}

TEST(engine, deleteLinesManyAtMiddleOfBufferWithManyLinesWhereDeletionGoesOffTheEnd) {
    // TODO(TB): when the deletion goes past the end, should it fail and not change the buffer, or succeed and delete as
    // much as possible?
    test_delete_lines({string_1, string_2, string_3}, 3, 1, {string_1}, true);
}

TEST(engine, deleteLinesManyAtMiddleOfBufferWithManyLinesWhereDeletionFitsExactly) {
    test_delete_lines({string_1, string_2, string_3, string_4}, 3, 1, {string_1}, true);
}

TEST(engine, deleteLinesManyAtMiddleOfBufferWithManyLinesWhereDeletionFits) {
    test_delete_lines({string_1, string_2, string_3, string_4, string_5}, 3, 1, {string_1, string_5}, true);
}

TEST(engine, deleteLinesZeroAtEndOfBufferWithManyLines) {
    test_delete_lines({string_1, string_2, string_3}, 0, 2, {string_1, string_2, string_3}, true);
}

TEST(engine, deleteLinesOneAtEndOfBufferWithManyLinesWhereDeletionFitsExactly) {
    test_delete_lines({string_1, string_2, string_3}, 1, 2, {string_1, string_2}, true);
}

TEST(engine, deleteLinesManyAtEndOfBufferWithManyLinesWhereDeletionGoesOffTheEnd) {
    // TODO(TB): when the deletion goes past the end, should it fail and not change the buffer, or succeed and delete as
    // much as possible?
    test_delete_lines({string_1, string_2, string_3}, 3, 2, {string_1, string_2}, true);
}
// #endregion

// #region void delete_character(Buffer&, const Length line_index, const Length character_index);
static void test_delete_character(std::vector<std::string> lines,
    tte::Length line_index,
    tte::Length character_index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);
    ASSERT_EQ(tte::engine::delete_character(buffer, line_index, character_index), expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, deleteCharacterAtInvalidLineIndexOfEmptyBuffer) { test_delete_character({}, 0, 0, {}, false); }

TEST(engine, deleteCharacterAtInvalidLineIndexOfNonEmptyBuffer) {
    test_delete_character({string_1}, 1, 0, {string_1}, false);
}

TEST(engine, deleteCharacterAtInvalidCharacterIndexOfEmptyLine) {
    test_delete_character({string_1, empty_string, string_3}, 1, 0, {string_1, empty_string, string_3}, false);
}

TEST(engine, deleteCharacterAtInvalidCharacterIndexOfNonEmptyLine) {
    test_delete_character({string_1, string_2, string_3}, 1, strlen(string_2), {string_1, string_2, string_3}, false);
}

TEST(engine, deleteCharacterAtBeginningOfLineWithOneCharacter) {
    test_delete_character({string_1, "a", string_3}, 1, 0, {string_1, "", string_3}, true);
}

TEST(engine, deleteCharacterAtBeginningOfLineWithManyCharacters) {
    test_delete_character({string_1, "abc", string_3}, 1, 0, {string_1, "bc", string_3}, true);
}

TEST(engine, deleteCharacterAtMiddleOfLineWithManyCharacters) {
    test_delete_character({string_1, "abc", string_3}, 1, 1, {string_1, "ac", string_3}, true);
}

TEST(engine, deleteCharacterAtEndOfLineWithManyCharacters) {
    test_delete_character({string_1, "abc", string_3}, 1, 2, {string_1, "ab", string_3}, true);
}
// #endregion

// #region void delete_characters(Buffer&, const Length number_of_characters, const Length line_index, const Length
// character_index);
static void test_delete_characters(std::vector<std::string> lines,
    tte::Length number_of_characters,
    tte::Length line_index,
    tte::Length character_index,
    std::vector<std::string> expected_lines,
    bool expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);
    ASSERT_EQ(tte::engine::delete_characters(buffer, number_of_characters, line_index, character_index),
        expected_result);
    assert_buffer_state(buffer, expected_lines);
}

TEST(engine, deleteCharactersZeroAtInvalidLineIndexOfEmptyBuffer) { test_delete_characters({}, 0, 0, 0, {}, true); }

TEST(engine, deleteCharactersZeroAtInvalidLineIndexOfNonEmptyBuffer) {
    test_delete_characters({string_1}, 0, 1, 0, {string_1}, true);
}

TEST(engine, deleteCharactersZeroAtInvalidCharacterIndexOfEmptyLine) {
    test_delete_characters({string_1, empty_string, string_3}, 0, 1, 0, {string_1, empty_string, string_3}, true);
}

TEST(engine, deleteCharactersZeroAtInvalidCharacterIndexOfNonEmptyLine) {
    test_delete_characters({string_1, string_2, string_3},
        0,
        1,
        strlen(string_2),
        {string_1, string_2, string_3},
        true);
}

TEST(engine, deleteCharactersZeroAtBeginningOfLineWithOneCharacter) {
    test_delete_characters({string_1, "a", string_3}, 0, 1, 0, {string_1, "a", string_3}, true);
}

TEST(engine, deleteCharactersZeroAtBeginningOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 0, 1, 0, {string_1, "abc", string_3}, true);
}

TEST(engine, deleteCharactersZeroAtMiddleOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 0, 1, 1, {string_1, "abc", string_3}, true);
}

TEST(engine, deleteCharactersZeroAtEndOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 0, 1, 2, {string_1, "abc", string_3}, true);
}

TEST(engine, deleteCharactersOneAtInvalidLineIndexOfEmptyBuffer) { test_delete_characters({}, 1, 0, 0, {}, false); }

TEST(engine, deleteCharactersOneAtInvalidLineIndexOfNonEmptyBuffer) {
    test_delete_characters({string_1}, 1, 1, 0, {string_1}, false);
}

TEST(engine, deleteCharactersOneAtInvalidCharacterIndexOfEmptyLine) {
    test_delete_characters({string_1, empty_string, string_3}, 1, 1, 0, {string_1, empty_string, string_3}, false);
}

TEST(engine, deleteCharactersOneAtInvalidCharacterIndexOfNonEmptyLine) {
    test_delete_characters({string_1, string_2, string_3},
        1,
        1,
        strlen(string_2),
        {string_1, string_2, string_3},
        false);
}

TEST(engine, deleteCharactersOneAtBeginningOfLineWithOneCharacter) {
    test_delete_characters({string_1, "a", string_3}, 1, 1, 0, {string_1, "", string_3}, true);
}

TEST(engine, deleteCharactersOneAtBeginningOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 1, 1, 0, {string_1, "bc", string_3}, true);
}

TEST(engine, deleteCharactersOneAtMiddleOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 1, 1, 1, {string_1, "ac", string_3}, true);
}

TEST(engine, deleteCharactersOneAtEndOfLineWithManyCharacters) {
    test_delete_characters({string_1, "abc", string_3}, 1, 1, 2, {string_1, "ab", string_3}, true);
}

TEST(engine, deleteCharactersManyAtInvalidLineIndexOfEmptyBuffer) { test_delete_characters({}, 3, 0, 0, {}, false); }

TEST(engine, deleteCharactersManyAtInvalidLineIndexOfNonEmptyBuffer) {
    test_delete_characters({string_1}, 3, 1, 0, {string_1}, false);
}

TEST(engine, deleteCharactersManyAtInvalidCharacterIndexOfEmptyLine) {
    test_delete_characters({string_1, empty_string, string_3}, 3, 1, 0, {string_1, empty_string, string_3}, false);
}

TEST(engine, deleteCharactersManyAtInvalidCharacterIndexOfNonEmptyLine) {
    test_delete_characters({string_1, string_2, string_3},
        3,
        1,
        strlen(string_2),
        {string_1, string_2, string_3},
        false);
}

TEST(engine, deleteCharactersManyAtBeginningOfLineWithOneCharacterWhereDeletionDoesNotFit) {
    test_delete_characters({string_1, "a", string_3}, 3, 1, 0, {string_1, "", string_3}, true);
}

TEST(engine, deleteCharactersManyAtBeginningOfLineWithManyCharactersWhereDeletionFitsExactly) {
    test_delete_characters({string_1, "abc", string_3}, 3, 1, 0, {string_1, "", string_3}, true);
}

TEST(engine, deleteCharactersManyAtBeginningOfLineWithManyCharactersWhereDeletionFits) {
    test_delete_characters({string_1, "abcde", string_3}, 3, 1, 0, {string_1, "de", string_3}, true);
}

TEST(engine, deleteCharactersManyAtMiddleOfLineWithManyCharactersWhereDeletionDoesNotFit) {
    test_delete_characters({string_1, "abc", string_3}, 3, 1, 1, {string_1, "a", string_3}, true);
}

TEST(engine, deleteCharactersManyAtMiddleOfLineWithManyCharactersWhereDeletionFitsExactly) {
    test_delete_characters({string_1, "abcd", string_3}, 3, 1, 1, {string_1, "a", string_3}, true);
}

TEST(engine, deleteCharactersManyAtMiddleOfLineWithManyCharactersWhereDeletionFits) {
    test_delete_characters({string_1, "abcdef", string_3}, 3, 1, 1, {string_1, "aef", string_3}, true);
}

TEST(engine, deleteCharactersManyAtEndOfLineWithManyCharactersWhereDeletionDoesNotFit) {
    test_delete_characters({string_1, "abc", string_3}, 3, 1, 2, {string_1, "ab", string_3}, true);
}
// #endregion

// #region const char* line_to_c_string(Buffer&, const Length line_index);
static void test_line_to_c_string(std::vector<std::string> lines, tte::Length index, const char* expected_result) {
    tte::engine::Buffer& buffer = create_buffer(lines);
    char* result = tte::engine::line_to_c_string(buffer, index);
    ASSERT_STREQ(result, expected_result);
    free(static_cast<void*>(result));
}

TEST(engine, lineToCStringAtInvalidIndexOfEmptyBuffer) { test_line_to_c_string({}, 0, nullptr); }

TEST(engine, lineToCStringAtInvalidIndexOfNonEmptyBuffer) {
    test_line_to_c_string({string_1, string_2, string_3}, 3, nullptr);
}

TEST(engine, lineToCStringAtBeginningOfBufferWithOneLineWithZeroCharacters) {
    test_line_to_c_string({empty_string}, 0, "");
}

TEST(engine, lineToCStringAtBeginningOfBufferWithOneLineWithOneCharacter) { test_line_to_c_string({"a"}, 0, "a"); }

TEST(engine, lineToCStringAtBeginningOfBufferWithOneLineWithManyCharacters) {
    test_line_to_c_string({"abc"}, 0, "abc");
}

TEST(engine, lineToCStringAtBeginningOfBufferWithManyLineWithZeroCharacters) {
    test_line_to_c_string({empty_string, string_1, string_2}, 0, "");
}

TEST(engine, lineToCStringAtBeginningOfBufferWithManyLineWithOneCharacter) {
    test_line_to_c_string({"a", string_1, string_2}, 0, "a");
}

TEST(engine, lineToCStringAtBeginningOfBufferWithManyLineWithManyCharacters) {
    test_line_to_c_string({"abc", string_1, string_2}, 0, "abc");
}

TEST(engine, lineToCStringAtMiddleOfBufferWithManyLineWithZeroCharacters) {
    test_line_to_c_string({string_1, empty_string, string_2}, 1, "");
}

TEST(engine, lineToCStringAtMiddleOfBufferWithManyLineWithOneCharacter) {
    test_line_to_c_string({string_1, "a", string_2}, 1, "a");
}

TEST(engine, lineToCStringAtMiddleOfBufferWithManyLineWithManyCharacters) {
    test_line_to_c_string({string_1, "abc", string_2}, 1, "abc");
}

TEST(engine, lineToCStringAtEndOfBufferWithManyLineWithZeroCharacters) {
    test_line_to_c_string({string_1, string_2, empty_string}, 2, "");
}

TEST(engine, lineToCStringAtEndOfBufferWithManyLineWithOneCharacter) {
    test_line_to_c_string({string_1, string_2, "a"}, 2, "a");
}

TEST(engine, lineToCStringAtEndOfBufferWithManyLineWithManyCharacters) {
    test_line_to_c_string({string_1, string_2, "abc"}, 2, "abc");
}
// #endregion

// #region const char* buffer_to_c_string(Buffer&);
TEST(engine, bufferToCStringWithEmptyBuffer) {
    tte::engine::Buffer& buffer = create_buffer({});
    char* result = tte::engine::buffer_to_c_string(buffer);
    ASSERT_STREQ(result, "");
    free(static_cast<void*>(result));
}

TEST(engine, bufferToCStringWithNonEmptyBuffer) {
    std::vector<std::string> lines{empty_string,
        string_1,
        string_3,
        empty_string,
        empty_string,
        string_2,
        string_1,
        string_1,
        string_4,
        empty_string,
        string_5,
        empty_string};
    std::string expected_result;
    for (std::string& line : lines) {
        expected_result += line + "\n";
    }
    tte::engine::Buffer& buffer = create_buffer({lines});
    char* result = tte::engine::buffer_to_c_string(buffer);
    ASSERT_STREQ(result, expected_result.c_str());
    free(static_cast<void*>(result));
}
// #endregion
