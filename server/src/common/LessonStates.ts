import {ZodDiscriminatedUnionOption, ZodTypeAny, z} from "zod";

const Base = z.object({
    type: z.string()
})

export const LessonIntro = Base.extend({
        type: z.literal("intro"),
        text: z.string()
    });

export const LessonSection = Base.extend({
        type: z.literal("section"),
        text: z.string(),
        photo: z.string().url().optional(),
    });

export const AnyLessonState = z.discriminatedUnion("type", [LessonIntro,LessonSection] )
export type AnyLessonStateType = z.infer<typeof AnyLessonState>;

export type LessonPlanType = AnyLessonStateType[];
