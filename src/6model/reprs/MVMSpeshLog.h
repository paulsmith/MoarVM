/* Representation used for holding data logged by the interpreter for the
 * purpose of producing specializations. */

/* The kind of log entry we have. */
typedef enum {
    /* Entry to a callframe. */
    MVM_SPESH_LOG_ENTRY,
    /* Parameter type information. */
    MVM_SPESH_LOG_PARAMETER,
    /* Parameter type if we were to decontainerize the parameter. Recorded
     * when the parameter is a container type. */
    MVM_SPESH_LOG_PARAMETER_DECONT,
    /* Decont, attribute lookup, or lexical lookup type information. */
    MVM_SPESH_LOG_TYPE,
    /* Static lexical lookup (bytecode says we can cache the result). */
    MVM_SPESH_LOG_STATIC,
    /* Invoked code object. */
    MVM_SPESH_LOG_INVOKE,
    /* OSR point. */
    MVM_SPESH_LOG_OSR
} MVMSpeshLogEntryKind;

/* Flags on types. */
#define MVM_SPESH_LOG_TYPE_FLAG_CONCRETE 1

/* An entry in the spesh log. */
struct MVMSpeshLogEntry {
    /* The kind of log entry it is; discriminator for the union. */
    MVMint32 kind;

    /* Call frame correlation ID. */
    MVMint32 id;

    union {
        /* Entry to a call frame (ENTRY). */
        struct {
            MVMStaticFrame *sf;
            MVMCallsite *cs;
        } entry;

        /* Observed parameter type (PARAMETER, PARAMETER_DECONT). */
        struct {
            MVMObject *type;
            MVMint32 flags;
            MVMuint16 arg_idx;
        } param;

        /* Observed type (TYPE). */
        struct {
            MVMObject *type;
            MVMint32 flags;
            MVMint32 bytecode_offset;
        } type;

        /* Observed value (STATIC, INVOKE). */
        struct {
            MVMObject *value;
            MVMint32 bytecode_offset;
        } value;

        /* Observed OSR point (OSR). */
        struct {
            MVMint32 bytecode_offset;
        } osr;
    };
};

/* The spesh log representation itself. */
struct MVMSpeshLogBody {
    /* The sending thread. */
    MVMThread *thread;

    /* Array of log entries. */
    MVMSpeshLogEntry *entries;

    /* Number of log entries so far and limit. */
    MVMuint32 used;
    MVMuint32 limit;

    /* When in debug mode, mutex and condition variable used to block the
     * thread sending a log until the spesh worker has processed it. */
    uv_mutex_t *block_mutex;
    uv_cond_t *block_condvar;
    AO_t completed;
};
struct MVMSpeshLog {
    MVMObject common;
    MVMSpeshLogBody body;
};

/* Function for REPR setup. */
const MVMREPROps * MVMSpeshLog_initialize(MVMThreadContext *tc);
