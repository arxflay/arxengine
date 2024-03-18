#include "arxengine/ArxException.h"
ARX_NAMESPACE_BEGIN

ArxException::ArxException()
    : ArxException(ErrorCode::GenericError)
{
}

ArxException::ArxException(ErrorCode errCode, std::string_view msg)
    : m_errCode(errCode), m_msg(msg)
{
}

std::string ArxException::ToString() const
{
   return "ErrorCode " + std::to_string(static_cast<int>(m_errCode)) + ", Message: " + m_msg;
}

std::unique_ptr<ArxException> ArxException::Clone() const { return std::make_unique<ArxException>(*this); }

std::string_view ArxException::GetMsg() const noexcept { return m_msg; }
ArxException::ErrorCode ArxException::GetErrorCode() const noexcept { return m_errCode; }
int ArxException::GetErrorCodeInt() const noexcept { return static_cast<int>(m_errCode); }

ARX_NAMESPACE_END
