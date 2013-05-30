/**
 * Test: Multiplexer: Loss only in async path
 *
 * Situation:
 * A plugin might register two listeners, one async and one sync. 
 *
 * Assert:
 * A testcase should be provided so that we loose some activities on the async path but
 * can verify that the sync path is uneffected.
 *
 */
