#pragma once

#define CheckNull(p) { if (p == nullptr) return; }
#define CheckNullResult(p, result) { if (p == nullptr) return result; }

#define CheckTrue(p) { if (p == true) return; }
#define CheckTrueResult(p) { if (p == true) return result; }

#define CheckFalse(p) { if (p == false) return; }
#define CheckFalseResult(p) { if (p == false) return result; }