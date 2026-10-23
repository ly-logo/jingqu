import http from './http'
import type { ApiResult, Rule } from './types'

export const ruleApi = {
  list(): Promise<ApiResult<Rule[]>> {
    return http.get('/rules').then((r) => r.data)
  },
  create(data: Omit<Rule, 'ruleId'>): Promise<ApiResult<Rule>> {
    return http.post('/rules', data).then((r) => r.data)
  },
  update(ruleId: number, data: Partial<Rule>): Promise<ApiResult<Rule>> {
    return http.put(`/rules/${ruleId}`, data).then((r) => r.data)
  },
  remove(ruleId: number): Promise<ApiResult<null>> {
    return http.delete(`/rules/${ruleId}`).then((r) => r.data)
  },
}
