import http from './http'
import type { ApiResult, User, UserCreateRequest } from './types'

export const userApi = {
  list(): Promise<ApiResult<User[]>> {
    return http.get('/users').then((r) => r.data)
  },
  create(data: UserCreateRequest): Promise<ApiResult<User>> {
    return http.post('/users', data).then((r) => r.data)
  },
  remove(userId: number): Promise<ApiResult<null>> {
    return http.delete(`/users/${userId}`).then((r) => r.data)
  },
}
