#include "spine/io/stream/implementations/mock.hpp"
#include "spine/io/stream/stream.hpp"

#include <unity.h>

#include <climits>
#include <cstdint>
#include <cstdlib>
#include <vector>

using namespace spn::structure;

namespace {

void ut_stream_basics() {
    const auto stream_cfg = spn::io::MockStream::Config{.input_buffer_size = 128, .output_buffer_size = 128};
    auto stream = spn::io::MockStream(std::move(stream_cfg));
    stream.initialize();

    const auto test_data = std::vector<uint8_t>{'a', 'b', 'c'};

    // do a write test
    stream.write(test_data.data(), test_data.size());
    auto bytes_in_output_buffer = stream.extract_bytestream();
    TEST_ASSERT_EQUAL(true, bool(bytes_in_output_buffer));
    TEST_ASSERT_EQUAL(test_data.size(), bytes_in_output_buffer->size());
    TEST_ASSERT_EQUAL(true, bytes_in_output_buffer == test_data);

    // do a read test
    stream.inject_bytestream(test_data);
    auto result_data = std::vector<uint8_t>(3);
    TEST_ASSERT_EQUAL(test_data.size(), stream.available());
    TEST_ASSERT_EQUAL(test_data.size(), stream.read(result_data.data(), result_data.size()));
    TEST_ASSERT_EQUAL(true, test_data == result_data);
}

} // namespace

int run_all_tests() {
    UNITY_BEGIN();
    RUN_TEST(ut_stream_basics);
    return UNITY_END();
}

#if defined(ARDUINO)
#    include <Arduino.h>
void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
    run_all_tests();
}

void loop() {}
#else
int main(int argc, char** argv) {
    run_all_tests();
    return 0;
}
#endif
