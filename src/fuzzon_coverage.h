
#ifndef FUZZON_COVERAGE_H_
#define FUZZON_COVERAGE_H_

#include <map>
#include <vector>

namespace fuzzon {

class Coverage {
public:
	enum TrackMode
	{
		Raw,
		Flow
	};

	Coverage(TrackMode mode);
	virtual ~Coverage();


	enum CompreseMode
	{
		Log2
	};

	void Compress(CompreseMode comprese_mode = CompreseMode::Log2);

	bool operator==(const Coverage&) const;
	bool operator!=(const Coverage&) const;

	void TracePC(uintptr_t PC);
	void PrintTrace() const;

private:
	std::map<uintptr_t, unsigned int> pc_counter_;
	std::vector<uintptr_t> pc_trace_;
	uintptr_t lact_pc_;
	TrackMode mode_;

	static const size_t pc_flow_size = 64 * 1024;
	uint32_t pc_flow_[pc_flow_size / (sizeof(uint32_t))];


	std::string PrintPCTraces() const;
	std::string PrintPCConuters() const;
	std::string PrintPCFlow() const;
};

} /* namespace fuzzon */

#endif /* FUZZON_COVERAGE_H_ */
