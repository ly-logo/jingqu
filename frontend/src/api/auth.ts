import http from './http'
import type { ApiResult, LoginRequest, LoginResponse } from './types'

export const authApi = {
  login(data: LoginRequest): Promise<ApiResult<LoginResponse>> {
    return http.post('/auth/login', data).then((r) => r.data)
  },
  register(data: LoginRequest): Promise<ApiResult<LoginResponse>> {
    return http.post('/auth/register', data).then((r) => r.data)
  },
}
