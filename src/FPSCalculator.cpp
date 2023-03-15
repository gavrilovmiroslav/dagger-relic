#include "FPSCalculator.h"

void CountFPSCalculatorModule::process_signal(core::FrameDurationSignal& signal)
{
	delta_sum += signal.frame_duration;
	fps++;

	if (delta_sum >= refresh_freq)
	{
		submit_diagnostics();
		fps = 0;
		delta_sum -= refresh_freq;
	}
}

void SMAFPSCalculatorModule::process_signal(core::FrameDurationSignal& signal)
{
	duration_sum -= duration_list[current_index];  /* subtract value falling off */
	duration_sum += signal.frame_duration;              /* add new value */
	duration_list[current_index] = signal.frame_duration;   /* save new value so it can be subtracted later */
	current_index = (current_index + 1) % MAXSAMPLES;

	delta_sum += signal.frame_duration;
	if (delta_sum >= refresh_freq)
	{
		fps = MAXSAMPLES / duration_sum;
		submit_diagnostics();
		delta_sum -= refresh_freq;
	}
}
