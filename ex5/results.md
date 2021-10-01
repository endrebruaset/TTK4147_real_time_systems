### Task A and B
- Running with all four cores gave a much better worst-case execution time than running on a single core (1600us against 26000us).
- The fastest response times are down towards zero when using a single core, but about 10us when using four cores.
- The shape of the distribution when using four cores hass less variance than when only using a single core. A reason for this might be
  becuase a delay in one of the responses affects all three in the single core case, whereas each thread only affects itself when using four cores.

### Task C and D
- Without disturbance, the busy waiting has the fastest average response time. However, depending on the period of the periodic polling,
  the response times of the periodic setup is not much worse. For instance, using a period of 100ns works approximately as fast as busy waiting.
  A period of 1ms is significantly worse in terms of fastest response time.
- The periodic setup is far less affected by disturbance. This is becuase the use of clock_nanosleep forces the scheduler to prioritize it
  over the disturbance threads. In the busy waiting case, the disturbance threads block the response threads to a higher degree.
- With disturbance enabled, the busy waiting has an unpredictable worst-case exection time, whereas the periodic setup has a much more predictable
  worst-case execution time, given that the period is not too short. If the period is too short, however, we lose this predictability. Why?