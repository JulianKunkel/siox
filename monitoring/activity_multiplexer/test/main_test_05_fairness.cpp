/**
 * Test: Multiplexer: Fairness
 *
 * Situtation:
 * Activities may arrive from diffrents nets an an async manner.
 * Due to locks when handling arriving activities, synchronisation accurs.
 * This intrudoces problems of contentions.
 *
 * Assert:
 * A fair distribution between participating nets.
 *
 * BAD:  aaaaaababbbbbbba
 * GOOD: abababbaababbaba
 */
