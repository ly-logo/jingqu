import { defineStore } from 'pinia'
import { ref, computed } from 'vue'
import { login as apiLogin, register as apiRegister } from '@/api'
import type { LoginRequest } from '@/api'

export const useAuthStore = defineStore('auth', () => {
  const token = ref<string>(localStorage.getItem('token') || '')
  const role = ref<string>(localStorage.getItem('role') || '')
  const username = ref<string>(localStorage.getItem('username') || '')

  const isAdmin = computed(() => role.value === 'admin')

  function applySession(data: { token: string; role: string; username: string }) {
    token.value = data.token
    role.value = data.role
    username.value = data.username
    localStorage.setItem('token', data.token)
    localStorage.setItem('role', data.role)
    localStorage.setItem('username', data.username)
  }

  async function login(data: LoginRequest) {
    const res = await apiLogin(data)
    applySession(res.data)
    return res.data
  }

  async function register(data: LoginRequest) {
    const res = await apiRegister(data)
    applySession(res.data)
    return res.data
  }

  function logout() {
    token.value = ''
    role.value = ''
    username.value = ''
    localStorage.removeItem('token')
    localStorage.removeItem('role')
    localStorage.removeItem('username')
  }

  return { token, role, username, isAdmin, login, register, logout }
})
