#ifndef ASSERT_H
#define ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

extern void vcAssertFailure(const char *aFile, unsigned aLine);

#define assert(aCond) ((aCond) ? (void)0 : vcAssertFailure(__FILE__, __LINE__))

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H */
