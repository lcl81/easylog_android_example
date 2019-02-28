#include <stdio.h>
#include <stdlib.h>

#include "easylogging++.h"

static const char *easylog_prev_filename = "prev.log";
static const char *easylog_ids[] = {
    "logid_vcapture", "/data/easylog/logid_vcapture/curr.log",
    "logid_vencoder", "/data/easylog/logid_vencoder/curr.log",
    "logid_algrithm", "/data/easylog/logid_algrithm/curr.log",
    "logid_threadid", "/data/easylog/logid_threadid/curr.log",
};

static void easylog_configure(
    el::Logger &logger, const std::string &filename)
{
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    defaultConf.setGlobally(el::ConfigurationType::SubsecondPrecision, "3");
    defaultConf.setGlobally(el::ConfigurationType::PerformanceTracking, "false");
    defaultConf.setGlobally(el::ConfigurationType::MaxLogFileSize, "1048576"); // 1M
    defaultConf.setGlobally(el::ConfigurationType::LogFlushThreshold, "0");
    defaultConf.setGlobally(el::ConfigurationType::Format, "%logger: %msg");
    defaultConf.setGlobally(el::ConfigurationType::Filename, filename);
    logger.configure(defaultConf);
    return;
}

namespace zBaseUtil{
    #define BLK_SIZE 4096
    bool copy_file(const std::string &src, const std::string &dst)
    {
        FILE *fp_src = fopen(src.c_str(), "rb");
        if (NULL == fp_src) {
            return false;
        }

        FILE *fp_dst = fopen(dst.c_str(), "wb");
        if (NULL == fp_dst) {
            (void)fclose(fp_src);
            return false;
        }

        char buf[BLK_SIZE];
        size_t size;
        while ((size = fread(buf, 1, BLK_SIZE, fp_src)))
        {
            fwrite(buf, 1, size, fp_dst);
        }

        (void)fclose(fp_src);
        (void)fclose(fp_dst);
        return true;
    }
}

static void rolloutHandler(const char* filename, std::size_t size)
{
    // printf("filename= %s, size=%d\n", filename, size);
    if (    nullptr == filename
        ||  size <= 0)
    {
        return;
    }
    
    std::string newfile = el::base::utils::File::extractPathFromFilename(filename, "/") + easylog_prev_filename;
    (void)zBaseUtil::copy_file(filename, newfile);
    return;
}

void easylog_init()
{
    // 创建并初始化日志记录器
    for (uint8_t i=0; i<(sizeof(easylog_ids) / sizeof(char *)); i+=2)
    {
        el::Logger* newLogger = el::Loggers::getLogger(easylog_ids[i]);
        easylog_configure(*newLogger, easylog_ids[i+1]);
    }

    el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);
    el::Loggers::addFlag(el::LoggingFlag::ImmediateFlush);
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::addFlag(el::LoggingFlag::DisableVModulesExtensions);

    /// 注册回调函数
    el::Helpers::installPreRollOutCallback(rolloutHandler);
    return;
}

void easylog_vcapture_log(const std::string &msg)
{
    CLOG(INFO, easylog_ids[0*2]) << msg;
    return;
}

void easylog_vencoder_log(const std::string &msg)
{
    CLOG(INFO, easylog_ids[1*2]) << msg;
    return;
}

void easylog_algrithm_log(const std::string &msg)
{
    CLOG(INFO, easylog_ids[2*2]) << msg;
    return;
}

void easylog_threadid_log(const std::string &msg)
{
    CLOG(INFO, easylog_ids[3*2]) << msg;
    return;
}

// 按easylog 文档，在main函数前 调用初始化宏
INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    easylog_init();

    easylog_vcapture_log("test thread capture!");
    easylog_vencoder_log("test thread vencoder!");
    easylog_algrithm_log("test thread algrithm!");
    easylog_threadid_log("test thread threadid!");
    return 0;
}

