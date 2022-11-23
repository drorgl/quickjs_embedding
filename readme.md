# QuickJS

Source - https://github.com/bellard/quickjs
https://bellard.org/quickjs/bench.html
https://bellard.org/quickjs/


# Initialization
```c
JSContext *ctx;

JSRuntime *rt = JS_NewRuntime();
if (!rt) {
    return nullptr;
}

JS_SetMemoryLimit(rt, 64 * 1024 * 1024);
JS_SetGCThreshold(rt, 16 * 1024 * 1024);

ctx = JS_NewContext(rt);

if (!ctx) {
    JS_FreeRuntime(rt);
    return nullptr;
}

interpreter->backend_data = ctx;
	
```

# Free
```c
JS_FreeContext(ctx);
```

# ToString
```c
str = JS_ToCString(ctx, val);
JS_FreeCString(ctx, str);
```

# Error Handling
```c
JS::SetWarningReporter(ctx, error_reporter);
```

# Handle Timeout
```c
JS_SetContextOpaque(ctx, interpreter);
JS_SetInterruptHandler(JS_GetRuntime(context.ctx), timeout_checker, &info);

int timeout_checker(JSRuntime *rt, void *opaque)
{
    script_info info = *static_cast<script_info*>(opaque);
    if(info.timeout != 0 && time(NULL) >= info.begin_time + info.timeout) /// timeout reached
    {
        writeLog(0, "Script '" + info.name + "' has exceeded timeout " + std::to_string(info.timeout) + ", terminate now.", LOG_LEVEL_WARNING);
        return 1;
    }
    return 0;
}
```

# Evaluating
```c
JSValue r = JS_Eval(ctx, code->source, code->length, "", 0);

if (JS_IsException(r)) {
    error_reporter(interpreter, ctx);
}
JS_FreeValue(ctx, dest_prop);



if ((eval_flags & JS_EVAL_TYPE_MASK) == JS_EVAL_TYPE_MODULE) {
    /* for the modules, we compile then run to be able to set
        import.meta */
    val = JS_Eval(ctx, buf, buf_len, filename,
                    eval_flags | JS_EVAL_FLAG_COMPILE_ONLY);
    if (!JS_IsException(val)) {
        js_module_set_import_meta(ctx, val, true);
        val = JS_EvalFunction(ctx, val);
    }
} else {
    val = JS_Eval(ctx, buf, buf_len, filename, eval_flags);
}
if (JS_IsException(val)) {
    js_std_dump_error(ctx);
    ret = -1;
} else {
    ret = 0;
}
JS_FreeValue(ctx, val);
```

# JS Functions
```c
//JS_SetPropertyStr
//JS_EvalFunction
//JS_ReadObject
//JS_Call
//JS_Invoke
```

# C Functions
```c
JSValue func_obj = JS_NewCFunctionData(ctx, &qts_quickjs_to_c_callback, /* min argc */0, /* unused magic */0, /* func_data len */1, func_data);
if (name != NULL) {
    JS_DefinePropertyValueStr(ctx, func_obj, "name", JS_NewString(ctx, name), JS_PROP_CONFIGURABLE);
}
```

# Building QuickJS Utilities

## repl
```
build qjsc
qjsc -c -o repl.c -m repl.js
pio run -e native -vv -t exec -a -c -a -o -a repl.cccc -a -m -a lib/quickjs/repl.js
```

# References
- https://github.com/rkd77/elinks/blob/5e802064c907668e085573d9c2245fafd9b94750/src/ecmascript/mujs.cpp
- https://github.com/rkd77/elinks/blob/b2dba0b67bce419b27afe007812ffbaa78e9fe0a/src/ecmascript/quickjs.cpp
- https://fuchsia.googlesource.com/third_party/quickjs/+/5e232e978bd33eb5b6574169fd5a0cb7c6e30b16/run-test262.c
- https://github.com/grassel/quickjs_wrapcpp/blob/990d631e2866c9c298d98520308e87275e7c2c30/wrap/qjs_main.cpp
