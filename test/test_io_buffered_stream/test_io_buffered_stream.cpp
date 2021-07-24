#include "spine/io/stream/buffered_stream.hpp"
#include "spine/io/stream/implementations/mock.hpp"
#include "spine/io/stream/stream.hpp"
#include "spine/io/stream/transaction.hpp"

#include <unity.h>

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <vector>

using namespace spn::structure;

namespace {

void ut_buffered_stream_basics() {
    auto mock_stream_cfg = spn::io::MockStream::Config{.input_buffer_size = 128, .output_buffer_size = 128};
    auto mock_stream = std::make_shared<spn::io::MockStream>(std::move(mock_stream_cfg));
    const auto buffered_stream_cfg =
        spn::io::BufferedStream::Config{.input_buffer_size = 100, .output_buffer_size = 100, .delimiters = "\n"};
    auto buffered_stream = spn::io::BufferedStream(mock_stream, std::move(buffered_stream_cfg));

    mock_stream->initialize();

    const auto test_data = std::string("abc\ndef\nghi\n");
    const auto expected_data = std::vector<std::string>{"abc", "def", "ghi"};
    auto expected_bytestream = std::vector<uint8_t>(test_data.begin(), test_data.end());

    // const auto test_data = std::string("abc\n");
    // const auto expected_data = std::vector<std::string>{"abc"};

    // sanity checks
    TEST_ASSERT_EQUAL(0, buffered_stream.length_of_next_line());
    TEST_ASSERT_EQUAL(false, buffered_stream.has_line());

    // do a write test
    buffered_stream.buffered_write(test_data.data(), test_data.size());
    buffered_stream.push_out_data();

    auto bytestream = mock_stream->extract_bytestream();
    TEST_ASSERT(bool(bytestream));
    TEST_ASSERT_EQUAL(expected_bytestream.size(), bytestream->size());
    TEST_ASSERT_EQUAL(true, expected_bytestream == bytestream);
    TEST_ASSERT_EQUAL(0, buffered_stream.length_of_next_line());
    TEST_ASSERT_EQUAL(false, buffered_stream.has_line());

    // do a read test
    mock_stream->inject_bytestream(expected_bytestream);
    buffered_stream.pull_in_data();

    for (size_t i = 0; i < expected_data.size(); ++i) {
        const auto discovered_length = buffered_stream.length_of_next_line();
        TEST_ASSERT_EQUAL(expected_data[i].size() + 1, discovered_length); // + 1 for newline
        TEST_ASSERT_EQUAL(true, buffered_stream.has_line());

        const auto result_view = buffered_stream.get_next_line_view(discovered_length);
        TEST_ASSERT_EQUAL(true, bool(result_view));
        TEST_ASSERT_EQUAL_STRING(expected_data[i].c_str(), std::string(*result_view).c_str());

        auto result = std::string(discovered_length, '\0');
        result.resize(discovered_length);
        auto returned_length = buffered_stream.get_next_line(result.data(), result.size(), discovered_length);
        TEST_ASSERT_EQUAL(expected_data[i].size(), returned_length);
        TEST_ASSERT_EQUAL_STRING(expected_data[i].c_str(), result.c_str());
    }

    // sanity checks
    TEST_ASSERT_EQUAL(0, buffered_stream.length_of_next_line());
    TEST_ASSERT_EQUAL(false, buffered_stream.has_line());
}

void ut_buffered_stream_transaction() {
    auto mock_stream_cfg = spn::io::MockStream::Config{.input_buffer_size = 128, .output_buffer_size = 128};
    auto mock_stream = std::make_shared<spn::io::MockStream>(std::move(mock_stream_cfg));
    const auto buffered_stream_cfg =
        spn::io::BufferedStream::Config{.input_buffer_size = 100, .output_buffer_size = 100, .delimiters = "\n"};
    auto buffered_stream = spn::io::BufferedStream(mock_stream, std::move(buffered_stream_cfg));

    mock_stream->initialize();

    const auto test_data = std::string("abc\ndef\nghi\n");
    const auto expected_data = std::vector<std::string>{"abc", "def", "ghi"};
    auto expected_bytestream = std::vector<uint8_t>(test_data.begin(), test_data.end());

    mock_stream->inject_bytestream(expected_bytestream);
    buffered_stream.pull_in_data();

    { // a transaction which ends up committed
        auto transaction = buffered_stream.new_transaction();
        TEST_ASSERT(!transaction->is_finalized());
        TEST_ASSERT_EQUAL(true, bool(transaction));
        TEST_ASSERT_EQUAL(true, std::string_view("abc") == transaction->incoming());

        TEST_ASSERT_EQUAL(0, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(3, transaction->outgoing(std::string_view("jkl")));
        TEST_ASSERT_EQUAL(3, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(3, transaction->outgoing_space_used());

        transaction->commit();
        TEST_ASSERT(transaction->is_finalized());

        TEST_ASSERT_EQUAL(3, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(0, transaction->outgoing_space_used());

        TEST_ASSERT_EQUAL(3, buffered_stream.push_out_data());
        auto _ = mock_stream->extract_bytestream();
    }

    { // a transaction which ends up aborted
        auto transaction = buffered_stream.new_transaction();
        TEST_ASSERT_EQUAL(true, bool(transaction));
        TEST_ASSERT_EQUAL(true, std::string_view("def") == transaction->incoming());

        TEST_ASSERT_EQUAL(0, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(3, transaction->outgoing(std::string_view("jkl")));
        TEST_ASSERT_EQUAL(3, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(3, transaction->outgoing_space_used());

        transaction->abort();
        TEST_ASSERT(transaction->is_finalized());

        TEST_ASSERT_EQUAL(0, buffered_stream.output_buffer_space_used());
        TEST_ASSERT_EQUAL(0, transaction->outgoing_space_used());
    }
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_buffered_stream_basics);
    RUN_TEST(ut_buffered_stream_transaction);
    return UNITY_END();
}

#if defined(ARDUINO) && defined(EMBEDDED)
#    include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    run_all_tests();
}

void loop() {}
#elif defined(ARDUINO)
#    include <ArduinoFake.h>
#endif

int main(int argc, char** argv) {
#if defined(ARDUINO)
    ARDUINO_MOCK_ALL();
#endif
    run_all_tests();
    return 0;
}
