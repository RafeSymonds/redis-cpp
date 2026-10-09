# redis-cpp

Build your own Redis in C++. It's CodeCrafters' challenge, written by hand and run locally against the official tester.

```sh
bin/course show     # instructions for the next stage
bin/course test     # build, run that stage plus every stage already passed
bin/course list     # all 124 stages and progress
```

Progress: `course/progress.json`. Full stage instructions: `course/stages/`.

## Build your own Redis

[Redis](https://redis.io/docs/latest/develop/get-started/data-store/) is an in-memory data structure store often used as a database, cache, message broker and streaming engine. In this challenge
you'll build your own Redis server that is capable of serving basic commands, reading RDB files and more.

Along the way, you'll learn about TCP servers, the Redis Protocol and more.

## Stages

### Base (7 stages)

1. **Bind to a port** (very easy): In this stage, you'll start a TCP server on port 6379, which is the default port that Redis uses.
2. **Respond to PING** (easy): In this stage, you'll respond to the [PING](https://redis.io/commands/ping) command. You'll use [the Redis protocol](https://redis.io/topics/protocol) to encode the reply.
3. **Respond to multiple PINGs** (easy): In this stage, you'll respond to multiple [PING](https://redis.io/commands/ping) commands sent by the same client.
4. **Handle concurrent clients** (medium): In this stage, you'll add support for multiple concurrent clients to your Redis server. To achieve this you'll use an [Event Loop](https://en.wikipedia.org/wiki/Event_loop), like the official Redis implementation does.
5. **Implement the ECHO command** (medium): In this stage, you'll respond to the [ECHO](https://redis.io/commands/echo) command. You'll parse user input according to the [the Redis protocol specification](https://redis.io/topics/protocol).
6. **Implement the SET & GET commands** (medium): In this stage, you'll need to implement the [SET](https://redis.io/commands/set) & [GET](https://redis.io/commands/get) commands.
7. **Expiry** (medium): In this stage, you'll add support for setting a key with an expiry. The expiry is provided using the "PX" argument to the [SET](https://redis.io/commands/set) command.

### Extension: RDB Persistence (6 stages)

In this challenge extension you'll add [persistence][redis-persistence] support to your Redis implementation.

Along the way you'll learn about Redis's [RDB file format][rdb-file-format] and more.

[redis-persistence]: https://redis.io/docs/latest/operate/oss_and_stack/management/persistence/
[rdb-file-format]: https://github.com/sripathikrishnan/redis-rdb-tools/blob/548b11ec3c81a603f5b321228d07a61a0b940159/docs/RDB_File_Format.textile

8. **RDB file config** (easy): In this stage, you'll add support for reading the config values related to where RDB files are stored. You'll implement the `CONFIG GET` command.
9. **Read a key** (medium): In this stage, you'll add support for reading a key from an RDB file that contains a single key-value pair. You'll do this by implementing the `KEYS *` command.
10. **Read a string value** (medium): In this stage, you'll add support for reading the value of a key from an RDB file that contains a single key-value pair.
11. **Read multiple keys** (medium): In this stage, you'll add support for reading multiple keys from an RDB file. You'll do this by extending the `KEYS *` command to support multiple keys.
12. **Read multiple string values** (medium): In this stage, you'll add support for reading multiple string values from an RDB file.
13. **Read value with expiry** (medium): In this stage, you'll add support for reading values that have an expiry set.

### Extension: AOF Persistence (10 stages)

In this challenge extension, you'll add [AOF persistence][aof-persistence] support to your Redis implementation.

Along the way, you'll learn about Redis' AOF file format, and how Redis restores its database after crash.

[aof-persistence]: https://redis.io/docs/latest/operate/oss_and_stack/management/persistence/#aof-advantages

14. **Default AOF options** (easy): In this stage, you'll implement the default values for AOF-related options.
15. **AOF options from flags** (easy): In this stage, you'll accept the values of AOF configuration options from the command line flags.
16. **Create append-only directory** (easy): In this stage, you'll add support for creating an append-only directory when append-only mode is enabled.
17. **Create append-only file** (easy): In this stage, you'll add support for creating the append-only file when append-only mode is enabled.
18. **Create manifest file** (easy): In this stage, you'll add support for creating the manifest file when append-only mode is enabled.
19. **Write a single command** (hard): In this stage, you'll add support for writing a single modifying command to the append-only file after it is processed.
20. **Write multiple commands** (medium): In this stage, you'll add support for writing multiple commands to the append-only file.
21. **Filter write commands** (easy): In this stage, you'll add support for writing only the modifying commands to the append-only file.
22. **Replay a single command** (hard): In this stage, you'll add support for restoring the database by replaying a single command from the append-only file.
23. **Replay multiple commands** (medium): In this stage, you'll extend replay of the append-only file to support multiple commands.

### Extension: Replication (18 stages)

In this challenge extension you'll add support for [Replication][redis-replication] to your Redis implementation.

Along the way you'll learn about how Redis's leader-follower replication works, the [PSYNC][redis-psync-command] command and more.

[redis-replication]: https://redis.io/docs/latest/operate/oss_and_stack/management/replication/
[redis-psync-command]: https://redis.io/commands/psync/

24. **Configure listening port** (easy): In this stage, you'll add support for parsing the `--port` flag and starting Redis on a custom port.
25. **The INFO command** (easy): In this stage, you'll add support for the INFO command on the master.
26. **The INFO command on a replica** (medium): In this stage, you'll add support for the --replicaof arg and INFO command on the replica.
27. **Initial replication ID and offset** (easy): In this stage, you'll add support for reading a key from an RDB file that contains a single key-value pair. You'll do this by implementing the `KEYS *` command.
28. **Send handshake (1/3)** (easy): In this stage, you'll add support for starting the handshake from the Replica side.
29. **Send handshake (2/3)** (easy): In this stage, you'll add support for continuing the handshake from the Replica side, by sending REPLCONF.
30. **Send handshake (3/3)** (medium): In this stage, you'll add support for finishing the handshake from the Replica side, by sending PSYNC.
31. **Receive handshake (1/2)** (easy): In this stage, you'll add support for starting the handshake from the master side, by accepting REPLCONF.
32. **Receive handshake (2/2)** (easy): In this stage, you'll add support for accepting PSYNC, and starting a FULLRESYNC.
33. **Empty RDB transfer** (easy): In this stage, you'll add support for sending an empty RDB file to the replica. This is part of the "full resynchronization" process.
34. **Single-replica propagation** (medium): In this stage, you'll add support for finishing the sync handshake from the master side, by sending a RDB file.
35. **Multi-replica propagation** (hard): In this stage, you'll complete your implementation of Redis replication.
36. **Command processing** (hard): In this stage, you'll add support for processing commands received by the replica from the master.
37. **ACKs with no commands** (easy): In this stage, you'll add support for returning an ACK back to master as a response to GETACK.
38. **ACKs with commands** (medium): In this stage, you'll add support for returning an ACK back to master as a response to GETACK.
39. **WAIT with no replicas** (medium): In this stage, you'll start implementing the WAIT command on your master.
40. **WAIT with no commands** (medium): In this stage, you'll continue implementing the WAIT command on your master.
41. **WAIT with multiple commands** (hard): In this stage, you'll finish implementing the WAIT command on your master.

### Extension: Streams (13 stages)

In this challenge extension you'll add support for the [Stream][redis-streams-data-type] data type to your Redis implementation.

Along the way you'll learn about commands like [XADD][xadd-command], [XRANGE][xrange-command] and more.

[redis-streams-data-type]: https://redis.io/docs/latest/develop/data-types/streams/
[xadd-command]: https://redis.io/commands/xadd/
[xrange-command]: https://redis.io/commands/xrange/

42. **The TYPE command** (easy): In this stage, you'll add support for the `TYPE` command.
43. **Create a stream** (medium): In this stage, you'll add support for creating a [Redis stream](https://redis.io/docs/latest/develop/data-types/streams/) using the `XADD` command.
44. **Validating entry IDs** (easy): In this stage, you'll enhance the `XADD` command by extending support for explicit IDs.
45. **Partially auto-generated IDs** (medium): In this stage, you'll enhance the `XADD` command by adding the option to use `*` as the sequence number.
46. **Fully auto-generated IDs** (medium): In this stage, you'll enhance the `XADD` command by adding the option to use `*` as the entry ID.
47. **Query entries from stream** (medium): In this stage, you'll add support for querying data from a stream using the `XRANGE` command.
48. **Query with -** (easy): In this stage, you'll extend support for `XRANGE` to allow querying using `-`.
49. **Query with +** (easy): In this stage, you'll extend support for `XRANGE` to allow querying using `+`.
50. **Query single stream using XREAD** (medium): In this stage, you'll add support to querying a stream using the `XREAD` command.
51. **Query multiple streams using XREAD** (medium): In this stage, you'll add extend support to `XREAD` to allow querying multiple streams.
52. **Blocking reads** (hard): In this stage, you'll add extend support to `XREAD` to allow querying multiple streams.
53. **Blocking reads without timeout** (medium): In this stage, you'll add extend support to `XREAD` to allow for the blocking command not timing out.
54. **Blocking reads using $** (easy): In this stage, you'll add extend support to `XREAD` to allow for passing in `$` as the ID for a blocking command.

### Extension: Transactions (11 stages)

In this challenge extension you'll add support for [Transactions][redis-transactions] to your Redis implementation.

Along the way, you'll learn about the [MULTI][multi-command], [EXEC][exec-command], and [DISCARD][discard-command] commands, as well as how Redis handles transactions atomically.

[redis-transactions]: https://redis.io/docs/latest/develop/interact/transactions/
[multi-command]: https://redis.io/commands/multi/
[exec-command]: https://redis.io/commands/exec/
[discard-command]: https://redis.io/commands/discard/

55. **The INCR command (1/3)** (easy): In this stage, you'll start implementing the INCR command.
56. **The INCR command (2/3)** (easy): In this stage, you'll continue implementing the INCR command.
57. **The INCR command (3/3)** (easy): In this stage, you'll finish implementing the INCR command.
58. **The MULTI command** (easy): In this stage, you'll implement the MULTI command.
59. **The EXEC command** (easy): In this stage, you'll start implementing the EXEC command.
60. **Empty transaction** (hard): In this stage, you'll implement an empty transaction.
61. **Queueing commands** (medium): In this stage, you'll implement queueing commands to a transaction.
62. **Executing a transaction** (hard): In this stage, you'll implement executing a successful transaction.
63. **The DISCARD command** (easy): In this stage, you'll implement the DISCARD command.
64. **Failures within transactions** (medium): In this stage, you'll implement handling failures while executing a transaction.
65. **Multiple transactions** (medium): In this stage, you'll implement multiple concurrent transactions.

### Extension: Optimistic Locking (8 stages)

In this challenge extension you'll add support for [Optimistic Locking][redis-optimistic-locking] to your Redis implementation.

Along the way you'll learn about the [WATCH][watch-command] command and how it works in synergy with the `MULTI`, `EXEC`, and `DISCARD` commands to achieve optimistic locking.

[redis-optimistic-locking]: https://redis.io/docs/latest/develop/using-commands/transactions/#cas
[watch-command]: https://redis.io/docs/latest/commands/watch/

66. **The WATCH command** (easy): In this stage, you'll add support for the `WATCH` command.
67. **WATCH inside transaction** (easy): In this stage, you'll add support for disallowing the `WATCH` command in a transaction.
68. **Tracking key modifications** (medium): In this stage, you'll add support for failing transactions based on the modification of watched keys.
69. **Watching multiple keys** (medium): In this stage, you'll add support for watching multiple keys.
70. **Watching missing keys** (easy): In this stage, you'll add support for watching a missing key.
71. **The UNWATCH command** (easy): In this stage, you'll add support for the `UNWATCH` command.
72. **Unwatch on EXEC** (easy): In this stage, you'll add support clearing watched keys on `EXEC`.
73. **Unwatch on DISCARD** (easy): In this stage, you'll add support clearing watched keys on `DISCARD`.

### Extension: Lists (11 stages)

In this challenge extension you'll add support for [Lists][redis-lists] to your Redis implementation.

Along the way, you'll learn commands like [RPUSH][rpush-command], [LRANGE][lrange-command], and more.

[redis-lists]: https://redis.io/docs/latest/develop/data-types/lists/
[rpush-command]: https://redis.io/docs/latest/commands/rpush/
[lrange-command]: https://redis.io/docs/latest/commands/lrange/

74. **Create a list** (easy): In this stage, you'll add support for creating a new list using the `RPUSH` command.
75. **Append an element** (easy): In this stage, you'll add support for `RPUSH` when a list already exists and a single element is being appended.
76. **Append multiple elements** (easy): In this stage, you'll add support for appending multiple elements in a single `RPUSH` command.
77. **List elements (positive indexes)** (easy): In this stage, you will add support for listing the elements of a list using the `LRANGE` command.
78. **List elements (negative indexes)** (easy): In this stage, you will add support for negative indexes for the `LRANGE` command.
79. **Prepend elements** (easy): In this stage, you'll add support for the `LPUSH` command, which prepends elements to a list.
80. **Query list length** (easy): In this stage, you'll add support for querying the length of a list using `LLEN`.
81. **Remove an element** (easy): In this stage, you'll implement support for removing a single element from the left using the `LPOP` command.
82. **Remove multiple elements** (easy): In this stage, you'll add support for removing multiple elements in a single `LPOP` command.
83. **Blocking retrieval** (medium): In this stage, you'll add support for the `BLPOP` command, which blocks until an element is available to be popped.
84. **Blocking retrieval with timeout** (medium): In this stage, you will add support for a non-zero timeout duration for the `BLPOP` command.

### Extension: Pub/Sub (7 stages)

In this challenge extension you'll add support for [Publish/Subscribe (Pub/Sub)][redis-pub-sub] to your Redis implementation.

Along the way, you'll learn commands like [SUBSCRIBE][subscribe-command], [PUBLISH][publish-command], and more.

[redis-pub-sub]: https://redis.io/docs/latest/develop/pubsub/
[subscribe-command]: https://redis.io/docs/latest/commands/subscribe/
[publish-command]: https://redis.io/docs/latest/commands/publish/

85. **Subscribe to a channel** (easy): In this stage, you’ll add support for the `SUBSCRIBE` command.
86. **Subscribe to multiple channels** (easy): In this stage, you'll add support for subscribing to multiple channels using the `SUBSCRIBE` command.
87. **Enter subscribed mode** (medium): In this stage, you'll add support for marking a client as having entered Subscribed mode.
88. **PING in subscribed mode** (easy): In this stage, you'll add support for responding to `PING` when a client is in subscribed mode.
89. **Publish a message** (easy): In this stage, you'll add support for the `PUBLISH` command.
90. **Deliver messages** (hard): In this stage, you will add support for delivering published messages to subscribed clients.
91. **Unsubscribe** (medium): In this stage, you'll add support for the `UNSUBSCRIBE command`, which is used to unsubscribe from a channel.

### Extension: Sorted Sets (8 stages)

In this challenge extension you'll add support for [Sorted Sets (zsets)][redis-zset] to your Redis implementation.

Along the way, you'll learn commands like [ZADD][zadd-command], [ZRANGE][zrange-command], and more.

[redis-zset]: https://redis.io/docs/latest/develop/data-types/sorted-sets/
[zadd-command]: https://redis.io/docs/latest/commands/zadd/
[zrange-command]: https://redis.io/docs/latest/commands/zrange/

92. **Create a sorted set** (easy): In this stage, you'll add support for creating a [sorted set](https://redis.io/docs/latest/develop/data-types/sorted-sets/) using the `ZADD` command.
93. **Add members** (medium): In this stage, you'll add support for adding elements to an existing sorted set.
94. **Retrieve member rank** (medium): In this stage, you'll add support for retrieving the rank of a sorted set member using the `ZRANK` command.
95. **List sorted set members** (easy): In this stage, you'll add support for listing the members of a sorted set using the `ZRANGE` command.
96. **ZRANGE with negative indexes** (easy): In this stage, you'll add support for negative indexes for the `ZRANGE` command.
97. **Count sorted set members** (easy): In this stage, you'll add support for counting the number of members in a sorted set using the `ZCARD` command.
98. **Retrieve member score** (medium): In this stage, you'll add support for retrieving the score of a sorted set member using the `ZSCORE` command.
99. **Remove a member** (easy): In this stage, you'll add support for removing a member of a sorted set using the `ZREM` command.

### Extension: Bitmaps (9 stages)

In this challenge extension you'll add support for [Bitmaps][redis-bitmap] to your Redis implementation.

Along the way, you'll learn commands like [SETBIT][setbit-command], [GETBIT][getbit-command], [BITCOUNT][bitcount-command], and more.

[redis-bitmap]: https://redis.io/docs/latest/develop/data-types/bitmaps/
[setbit-command]: https://redis.io/docs/latest/commands/setbit/
[getbit-command]: https://redis.io/docs/latest/commands/getbit/
[bitcount-command]: https://redis.io/docs/latest/commands/bitcount/

100. **Create a bitmap** (medium): In this stage, you'll add support for creating a bitmap using the [`SETBIT`](https://redis.io/docs/latest/commands/setbit/) command.
101. **Retrieve a bit** (easy): In this stage, you'll add support for retrieving a bit in a bitmap using the [`GETBIT`](https://redis.io/docs/latest/commands/getbit/) command.
102. **Read a string as bits** (medium): In this stage, you'll add support for using `GETBIT` on a key created with `SET`.
103. **Read bits as a string** (easy): In this stage, you'll add support for using `GET` on a key created with `SETBIT`.
104. **Grow a bitmap** (easy): In this stage, you'll add support for growing a bitmap with `SETBIT` and check its length with `STRLEN`.
105. **Count set bits** (medium): In this stage, you'll add support for counting set bits in a bitmap using the [`BITCOUNT`](https://redis.io/docs/latest/commands/bitcount/) command.
106. **AND two bitmaps** (medium): In this stage, you'll add support for combining bitmaps with [`BITOP AND`](https://redis.io/docs/latest/commands/bitop/).
107. **AND bitmaps of different lengths** (medium): In this stage, you'll add support for `BITOP AND` when the source bitmaps have different lengths.
108. **OR two bitmaps** (easy): In this stage, you'll add support for combining bitmaps with [`BITOP OR`](https://redis.io/docs/latest/commands/bitop/).

### Extension: Geospatial Commands (8 stages)

In this challenge extension you'll add support for [Geospatial Commands][geospatial-commands] to your Redis implementation.

Along the way, you'll learn commands like [GEOADD][geoadd-command], [GEOSEARCH][geosearch-command], and more.

[geospatial-commands]: https://redis.io/docs/latest/develop/data-types/geospatial/
[geoadd-command]: https://redis.io/docs/latest/commands/geoadd/
[geosearch-command]: https://redis.io/docs/latest/commands/geosearch/

109. **Respond to GEOADD** (easy): In this stage, you'll add support for responding to the `GEOADD` command.
110. **Validate coordinates** (easy): In this stage, you'll add support for validating the latitude and longitude provided in the `GEOADD` command.
111. **Store a location** (medium): In this stage, you'll add support for storing a location in a sorted set.
112. **Calculate location score** (hard): In this stage, you'll add support for calculating the score of a location.
113. **Respond to GEOPOS** (easy): In this stage, you'll add support for responding to the `GEOPOS` command.
114. **Decode coordinates** (hard): In this stage, you'll add support for decoding the coordinates of a location.
115. **Calculate distance** (medium): In this stage, you'll add support for calculating the distance between two locations using the `GEODIST` comamnd.
116. **Search within radius** (easy): In this stage, you'll add support for searching locations near a coordinate within a given radius using the `GEOSEARCH` command.

### Extension: Authentication (8 stages)

In this challenge extension you'll add support for default user [Authentication][redis-authentication] to your Redis implementation.

Along the way, you'll learn about commands like [ACL WHOAMI][acl-whoami-command], [ACL GETUSER][acl-getuser-command], [ACL SETUSER][acl-setuser-command], and [AUTH][auth-command], and more.

[redis-authentication]: https://redis.io/docs/latest/operate/oss_and_stack/management/security/
[acl-whoami-command]: https://redis.io/docs/latest/commands/acl-whoami/
[acl-getuser-command]: https://redis.io/docs/latest/commands/acl-getuser/
[acl-setuser-command]: https://redis.io/docs/latest/commands/acl-setuser/
[auth-command]: https://redis.io/docs/latest/commands/auth/

117. **Respond to ACL WHOAMI** (easy): In this stage, you'll add support for responding to the `ACL WHOAMI` command.
118. **Respond to ACL GETUSER** (easy): In this stage, you'll add support for responding to the `ACL GETUSER` command.
119. **The nopass flag** (easy): In this stage, you'll add support for responding to the `ACL GETUSER` command with the `nopass` flag set.
120. **The passwords property** (easy): In this stage, you'll add support for responding to the `ACL GETUSER` command with the passwords property.
121. **Setting default user password** (medium): In this stage, you'll add support for setting the default user's password.
122. **The AUTH command** (medium): In this stage, you'll add support for responding to the `AUTH` command.
123. **Enforce authentication** (medium): In this stage, you'll add support for enforcing authentication for the `default` user.
124. **Authenticate using AUTH** (medium): In this stage, you'll implement enforcing authentication using the `AUTH` command.

---

Course content is MIT, from codecrafters-io/build-your-own-redis@6925149 (see `course/LICENSE-codecrafters`).
