#ifdef _DEBUG

#include "gmock/gmock.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

#else

#include <cstdio>

int main()
{
    printf("JsonParsing: parser/writer demo not implemented yet (see docs/phase2.md, docs/phase3.md).\n");
    return 0;
}

#endif
