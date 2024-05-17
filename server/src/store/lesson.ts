import { defineStore } from 'pinia'

export const useLessonStore = defineStore({
    id: 'lesson',
    state: () => ({
        count: 0
    }),
    getters: {
        isReady: (state) => {return false},
    },
    actions: {
        increment() {
            this.count++
        },
        attemptLoad(lesson_json:string): string|null {
            try {
                const raw_json = JSON.parse(lesson_json);
                console.log(raw_json);
                return "TO BE IMPLEMENTED"
            } 
            catch {
                return null;
            }
        }  
    }
})