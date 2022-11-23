

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#if defined(__APPLE__)
#include <malloc/malloc.h>
#elif defined(__linux__)
#include <malloc.h>
#endif

#include <quickjs.h>

#include "cutils.h"
#include "quickjs-libc.h"

#include <math.h>

// x64 5x

void time_c_function()
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    size_t iterations = 1000000;

    for (uint32_t i = 0; i < iterations; i++)
    {
        float a = i;
        float b = i;
        float value = (powf(a, 2) / sin(2 * M_PI / b)) - a / 2;
        (void)value;
    }

    end = clock();
    cpu_time_used = ((double)(end - start));
    printf("c %zu iterations took %f ms, %f per iteration, %f calcs per ms\r\n", iterations, cpu_time_used, cpu_time_used / iterations, iterations / cpu_time_used);
}

int main(int argc, char **argv)
{
    time_c_function();
    printf("creating runtime\r\n");
JSRuntime *rt = JS_NewRuntime();
if (!rt)
{
    fprintf(stderr, "cannot allocate JS runtime\n");
    exit(2);
}

JS_SetMemoryLimit(rt, 80 * 1024);
JS_SetMaxStackSize(rt, 10 * 1024);
printf("creating context\r\n");
JSContext *ctx = JS_NewContext(rt);
if (!ctx)
{
    fprintf(stderr, "cannot allocate JS context\n");
    exit(2);
}

    /*def transform(a,b):\r\n\
        return (a**2/math.sin(2*math.pi/b))-a/2\r\n\*/
    printf("evaluating expression\r\n");
const char *expr = "function transform(a,b){return  (a^2/Math.sin(2*Math.PI/b))-a/2;}";
JSValue r = JS_Eval(ctx, expr, strlen(expr), "", 0);
if (JS_IsException(r))
{
    printf("Error Evaluating Function\r\n");
}

printf("retrieving function\r\n");
JSValue global = JS_GetGlobalObject(ctx);
JSValue func = JS_GetPropertyStr(ctx, global, "transform");
if (JS_IsException(func))
{
    printf("Error Retrieving transform\r\n");
}

if (!JS_IsFunction(ctx, func))
{
    printf("is not function!\r\n");
}

    printf("calling function\r\n");
    clock_t start, end;
    double cpu_time_used;

    start = clock();

    size_t iterations = 1000000;

    for (size_t i = 0; i < iterations; i++)
    {
        JSValue args[2];
        args[0] = JS_NewFloat64(ctx, (double)i);
        args[1] = JS_NewFloat64(ctx, (double)i);
        JSValue res = JS_Call(ctx, func, global, 2, args);
        if (JS_IsException(res))
        {
            printf("Error Executing transform\r\n");
        }

        if (!JS_IsNumber(res))
        {
            printf("is not number!\r\n");
        }

        double result;
        if (JS_ToFloat64(ctx, &result, res))
        {
            printf("error parsing number\r\n");
        }

        (void)(result);
        // printf("result %f for %d\r\n", result, i);
        // JS_FreeValue(ctx, res);
        JS_FreeValue(ctx, args[0]);
        JS_FreeValue(ctx, args[1]);
    }

    end = clock();
    cpu_time_used = ((double)(end - start));
    printf("quickjs %zu iterations took %f ms, %f per iteration, %f calcs p/er ms\r\n", iterations, cpu_time_used, cpu_time_used / iterations, iterations / cpu_time_used);

    JS_FreeValue(ctx, global);
    JS_FreeValue(ctx, func);

    JS_FreeValue(ctx, r);

    JSMemoryUsage stats;
    JS_ComputeMemoryUsage(rt, &stats);
    JS_DumpMemoryUsage(stdout, &stats, rt);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

void app_main()
{
    main(0, NULL);
}