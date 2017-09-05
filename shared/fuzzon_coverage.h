#ifndef FUZZON_COVERAGE_H_
#define FUZZON_COVERAGE_H_

#include <stdint.h>

namespace fuzzon {

class Coverage {
public:
	enum TrackMode
	{
		Raw,
		Flow
	};

	enum CompreseMode
	{
		Log2
	};

	Coverage(TrackMode mode);
//	Coverage(Coverage& copy_me);

	void Compress(CompreseMode comprese_mode = CompreseMode::Log2);

	bool operator==(const Coverage&) const;
	bool operator!=(const Coverage&) const;

	void TracePC(uintptr_t PC);
	void PrintTrace() const;

//private:
	uintptr_t lact_pc_;
	TrackMode mode_;

	static const int pc_flow_size = 64 * 1024;
	uint32_t pc_flow_[pc_flow_size / (sizeof(uint32_t))];
};

} /* namespace fuzzon */

#endif /* FUZZON_COVERAGE_H_ */
